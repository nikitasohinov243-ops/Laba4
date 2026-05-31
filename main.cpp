#include "document.h"
#include "engine.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <set>
// g++ -std=c++17 main.cpp -o tfidf_lab
// ./tfidf_lab
int main() {
    std::ifstream doc_list_file("documents.txt");
    if (!doc_list_file.is_open()) {
        std::cout << "Error: cannot open documents.txt" << std::endl;
        return 1;
    }
    std::vector<Document> docs;
    std::unordered_map<std::string, std::set<std::string>> word_to_docs;
    std::string doc_name;
    while (std::getline(doc_list_file, doc_name)) {
        if (doc_name.empty()) continue;
        std::ifstream doc_file(doc_name);
        if (!doc_file.is_open()) continue;
        Document doc;
        doc.name = doc_name;
        std::string raw_word;
        while (doc_file >> raw_word) {
            std::string cleaned = clean_word(raw_word);
            if (!cleaned.empty()) {
                doc.word_counts[cleaned]++;
                doc.unique_words.insert(cleaned);
                doc.total_words++;
            }
        }
        for (const auto& w : doc.unique_words) {
            word_to_docs[w].insert(doc_name);
        }
        docs.push_back(doc);
    }
    doc_list_file.close();
    std::string command_line;
    while (std::getline(std::cin, command_line)) {
        if (command_line.empty()) continue;
        std::stringstream ss(command_line);
        std::string cmd;
        ss >> cmd;
        if (cmd == "WORD") {
            std::string word;
            if (ss >> word) {
                handle_word(docs, word_to_docs, clean_word(word));
            } else {
                std::cout << "Invalid query" << std::endl;
            }
        } else if (cmd == "WORD_IN_DOC") {
            std::string word, doc;
            if (ss >> word >> doc) {
                handle_word_in_doc(docs, word_to_docs, clean_word(word), doc);
            } else {
                std::cout << "Invalid query" << std::endl;
            }
        } else if (cmd == "DOC") {
            std::string doc;
            if (ss >> doc) {
                handle_doc(docs, word_to_docs, doc);
            } else {
                std::cout << "Invalid query" << std::endl;
            }
        } else if (cmd == "QUERY") {
            std::vector<std::string> query_words;
            std::string raw_word;
            std::string full_query_str = "";
            bool first = true;
            while (ss >> raw_word) {
                if (!first) {
                    full_query_str += " ";
                }
                full_query_str += raw_word;
                first = false;
                std::string cleaned = clean_word(raw_word);
                if (!cleaned.empty()) {
                    query_words.push_back(cleaned);
                }
            }
            handle_query(docs, word_to_docs, query_words, full_query_str);
        } else {
            std::cout << "Invalid query" << std::endl;
        }
    }
    return 0;
}