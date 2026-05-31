#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <cctype>

struct Document {
    std::string name;
    int total_words = 0;
    std::unordered_map<std::string, int> word_counts;
    std::set<std::string> unique_words;
};

inline std::string clean_word(const std::string& word) {
    std::string filtered = "";
    for (char c : word) {
        if (!std::ispunct(static_cast<unsigned char>(c))) {
            filtered += c;
        }
    }
    std::transform(filtered.begin(), filtered.end(), filtered.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return filtered;
}

#endif