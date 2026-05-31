#ifndef ENGINE_H
#define ENGINE_H

#include "document.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <iomanip>

inline double compute_tf(const Document& doc, const std::string& word) {
    auto it = doc.word_counts.find(word);
    if (it == doc.word_counts.end() || doc.total_words == 0) {
        return 0.0;
    }
    return static_cast<double>(it->second) / doc.total_words;
}

inline double compute_idf(size_t total_docs, size_t docs_with_word) {
    if (docs_with_word == 0) {
        return 0.0;
    }
    return std::log(static_cast<double>(total_docs) / docs_with_word);
}

inline void handle_word(const std::vector<Document>& docs, const std::unordered_map<std::string, std::set<std::string>>& word_to_docs, const std::string& word) {
    std::cout << "Word: " << word << "\n";
    std::cout << "Documents total: " << docs.size() << "\n";
    size_t df = 0;
    auto it = word_to_docs.find(word);
    if (it != word_to_docs.end()) {
        df = it->second.size();
    }
    std::cout << "Documents with word: " << df << "\n";
    std::cout << "IDF: " << std::fixed << std::setprecision(4) << compute_idf(docs.size(), df) << "\n";
    std::cout << "Appears in:\n";
    if (it != word_to_docs.end()) {
        for (const auto& doc_name : it->second) {
            std::cout << doc_name << "\n";
        }
    }
}

inline void handle_word_in_doc(const std::vector<Document>& docs, const std::unordered_map<std::string, std::set<std::string>>& word_to_docs, const std::string& word, const std::string& doc_name) {
    const Document* target_doc = nullptr;
    for (const auto& doc : docs) {
        if (doc.name == doc_name) {
            target_doc = &doc;
            break;
        }
    }
    std::cout << "Word: " << word << "\n";
    std::cout << "Document " << doc_name << "\n";
    if (!target_doc) {
        std::cout << "Count: 0\nTF: 0.0000\nTF-IDF: 0.0000\n";
        return;
    }
    int count = 0;
    auto it = target_doc->word_counts.find(word);
    if (it != target_doc->word_counts.end()) {
        count = it->second;
    }
    double tf = compute_tf(*target_doc, word);
    size_t df = 0;
    auto word_it = word_to_docs.find(word);
    if (word_it != word_to_docs.end()) {
        df = word_it->second.size();
    }
    double idf = compute_idf(docs.size(), df);
    std::cout << "Count: " << count << "\n";
    std::cout << "TF: " << std::fixed << std::setprecision(4) << tf << "\n";
    std::cout << "TF-IDF: " << std::fixed << std::setprecision(4) << (tf * idf) << "\n";
}

inline void handle_doc(const std::vector<Document>& docs, const std::unordered_map<std::string, std::set<std::string>>& word_to_docs, const std::string& doc_name) {
    const Document* target_doc = nullptr;
    for (const auto& doc : docs) {
        if (doc.name == doc_name) {
            target_doc = &doc;
            break;
        }
    }
    if (!target_doc) {
        std::cout << "Document: " << doc_name << "\nUnknown document\n";
        return;
    }
    std::cout << "Document: " << doc_name << "\n";
    std::cout << "Total words: " << target_doc->total_words << "\n";
    std::cout << "Unique words: " << target_doc->unique_words.size() << "\n";
    std::cout << "Top words:\n";
    std::vector<std::pair<std::string, double>> top_words;
    for (const auto& word : target_doc->unique_words) {
        double tf = compute_tf(*target_doc, word);
        size_t df = word_to_docs.at(word).size();
        double idf = compute_idf(docs.size(), df);
        top_words.push_back({word, tf * idf});
    }
    std::sort(top_words.begin(), top_words.end(), [](const auto& a, const auto& b) {
        if (std::abs(a.second - b.second) > 1e-9) {
            return a.second > b.second;
        }
        return a.first < b.first;
    });
    size_t limit = std::min(static_cast<size_t>(5), top_words.size());
    for (size_t i = 0; i < limit; ++i) {
        std::cout << (i + 1) << ". " << top_words[i].first << " (" << std::fixed << std::setprecision(4) << top_words[i].second << ")\n";
    }
}

inline void handle_query(const std::vector<Document>& docs, const std::unordered_map<std::string, std::set<std::string>>& word_to_docs, const std::vector<std::string>& query_words, const std::string& raw_query) {
    std::cout << "Query: " << raw_query << "\n";
    std::cout << "Results:\n";
    std::vector<std::pair<std::string, double>> results;
    for (const auto& doc : docs) {
        double total_score = 0.0;
        for (const auto& w : query_words) {
            double tf = compute_tf(doc, w);
            size_t df = 0;
            auto it = word_to_docs.find(w);
            if (it != word_to_docs.end()) {
                df = it->second.size();
            }
            double idf = compute_idf(docs.size(), df);
            total_score += (tf * idf);
        }
        if (total_score > 1e-9) {
            results.push_back({doc.name, total_score});
        }
    }
    std::sort(results.begin(), results.end(), [](const auto& a, const auto& b) {
        if (std::abs(a.second - b.second) > 1e-9) {
            return a.second > b.second;
        }
        return a.first < b.first;
    });
    for (size_t i = 0; i < results.size(); ++i) {
        std::cout << (i + 1) << ". " << results[i].first << " (" << std::fixed << std::setprecision(4) << results[i].second << ")\n";
    }
}

#endif