#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <kdd99/logreg_classifier.h>

#include "helpers.h"

using namespace kdd99;
using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <test.csv> <model.txt>" << endl;
        return EXIT_FAILURE;
    }

    std::ifstream model_file(argv[2]);
    auto all_coefs = read_vector(model_file);
    model_file.close();

    ifstream test_file(argv[1]);
    string first_line;
    getline(test_file, first_line);
    test_file.close();

    // количество признаков
    int num_features = 0;
    for (char c : first_line) {
        if (c == ',') num_features++;
    }

    // количество классов
    int coefs_per_class = num_features + 1;
    int num_classes = all_coefs.size() / coefs_per_class;

    // проверка, что разделилось нацело
    if (num_classes * coefs_per_class != all_coefs.size()) {
        cerr << "Error: Model file size inconsistent with feature count" << endl;
        return EXIT_FAILURE;
    }

    // создание бинарных классификаторов для каждого класса
    std::vector<LogregClassifier> classifiers;
    for (int i = 0; i < num_classes; i++) {
        vector<float> class_coefs (
            all_coefs.begin() + i * coefs_per_class,
            all_coefs.begin() + (i + 1) * coefs_per_class
        );
        classifiers.emplace_back(class_coefs);
    }

    ifstream test_data(argv[1]);
    string line;
    int correct = 0;
    int total = 0;

    while (std::getline(test_data, line)) {
        stringstream ss(line);
        string value;

        // правильный класс
        getline(ss, value, ',');
        int true_label = stoi(value);

        auto features = LogregClassifier::features_t{};
        while (getline(ss, value, ',')) {
            features.push_back(stof(value) / 255.0f);
        }

        // проверка, что количество признаков совпадает
        if (features.size() != num_features) {
            cerr << "Warning: Feature count mismatch in line " << total + 1 << endl;
            continue;
        }

        // Получаем предсказания от всех классификаторов
        std::vector<float> probs(num_classes);
        for (int i = 0; i < num_classes; i++) {
            probs[i] = classifiers[i].predict_proba(features);
        }
        
        // Выбираем класс с максимальной вероятностью
        int pred_label = std::max_element(probs.begin(), probs.end()) - probs.begin();
        
        if (pred_label == true_label) {
            correct++;
        }
        total++;
    }

    // Выводим accuracy
    if (total > 0) {
        printf("%.3f\n", (double)correct / total);
    } else {
        cerr << "No test data processed" << endl;
        return EXIT_FAILURE;
    }
}