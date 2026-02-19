#include <fstream>
#include <iostream>
#include <limits>

#include <gtest/gtest.h>

#include <kdd99/logreg_classifier.h>
#include <helpers.h>
#include <sstream>
#include <string>

using kdd99::LogregClassifier;
using std::clog;

TEST(LogregClassifier, compare_to_python) {
    std::ifstream istream{"train/logreg_coef.txt"};
    ASSERT_TRUE(istream.is_open());
    auto coef = read_vector(istream);
    istream.close();

    int num_features = 784;
    int coefs_per_class = num_features + 1;  // +1 для bias
    int num_classes = coef.size() / coefs_per_class;
    
    std::vector<LogregClassifier> classifiers;
    for (int i = 0; i < num_classes; i++) {
        std::vector<float> class_coefs(
            coef.begin() + i * coefs_per_class,
            coef.begin() + (i + 1) * coefs_per_class
        );
        classifiers.emplace_back(class_coefs);
    }

    // файл с ожидаемыми классами
    std::ifstream test_data{"train/test_data_logreg.txt"};
    ASSERT_TRUE(test_data.is_open());

    // файл с тестовыми данными
    std::ifstream test_csv{"test.csv"};
    ASSERT_TRUE(test_csv.is_open());

    std::string csv_line;
    int expected_class;
    int line_num = 0;
    int correct = 0;
    int total = 0;

    while (std::getline(test_csv, csv_line) && test_data >> expected_class) {
        line_num++;
        
        // игнорирование остальных чисел в строке test_data_logreg.txt
        test_data.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::stringstream ss(csv_line);
        std::string value;
        
        // Пропускаем первый столбец (настоящий класс) - он не нужен для predict_proba
        std::getline(ss, value, ',');
        int true_label = std::stoi(value);  // можно использовать для отладки
        
        // Читаем пиксели и нормализуем
        std::vector<float> features;
        while (std::getline(ss, value, ',')) {
            features.push_back(std::stof(value) / 255.0f);
        }
        
        // Получаем вероятности от всех классификаторов
        std::vector<float> probs(num_classes);
        for (int i = 0; i < num_classes; i++) {
            probs[i] = classifiers[i].predict_proba(features);
        }
        
        // Выбираем класс с максимальной вероятностью
        int pred_class = std::max_element(probs.begin(), probs.end()) - probs.begin();
        
        // Сравниваем предсказанный класс с ожидаемым из файла
        EXPECT_EQ(expected_class, pred_class) 
            << "Mismatch at line " << line_num 
            << "\n True label: " << true_label
            << "\n Expected class: " << expected_class
            << "\n Predicted class: " << pred_class;
        
        if (pred_class == true_label) {
            correct++;
        }
        total++;
    }
}
