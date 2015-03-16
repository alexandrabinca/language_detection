#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <vector>
#include <string>
#include <unordered_map>
#include <math.h>
#include <algorithm> 
#include <locale> 

using namespace std;
locale mylocale("");

//wide char: int; transition (2 wide chars): long long
unordered_map<string, unordered_map<long long, int>> lang_transition_occurrences;
unordered_map<string, unordered_map<int, int>> lang_char_occurrences;

double non_existing_penalty = 0.000001;
long long INF = 1000000;

string languages[] = {"english", "french", "german", "italian", "romanian", "spanish"};

// returns pair(transition_occurences, char_occurrences)
pair<unordered_map<long long, int>, unordered_map<int, int>> get_statistics(string fileName) {
  wfstream file;
  file.open(fileName, ios::in);
  file.imbue(mylocale);

  unordered_map<long long, int> transition_occurrences;
  unordered_map<int, int> char_occurrences;

  wstring wline;

  while (getline(file, wline)) {
    long long c1 = -1, c2;
    for (auto c : wline) {
      if (c == L'\0') continue;

      if (char_occurrences.find((int) c) == char_occurrences.end()) {
        char_occurrences[(int) c] = 0;
      }
      ++ char_occurrences[(int) c];

      if (c1 == -1) {
        c1 = (long long) c;
      } else {
        c2 = (long long) c;
        long long t = (c1 << 32) + c2;
        if (transition_occurrences.find(t) == transition_occurrences.end()) {
          transition_occurrences[t] = 0;
        }
        ++ transition_occurrences[t];
        c1 = c2;
      }
    }
  }

  file.close();
  return make_pair(transition_occurrences, char_occurrences);
}

double get_score(unordered_map<long long, int>& current_to, 
                  unordered_map<int, int> current_co, 
                  unordered_map<long long, int>& candidate_to,
                  unordered_map<int, int> candidate_co) {

  vector <double> v;
  for (auto& entry : current_to) {
    long long transition = entry.first;
    int first_letter = (int) (transition >> 32);
    if (candidate_to.find(transition) == candidate_to.end()) {
      v.push_back(non_existing_penalty * (double)current_to[transition]/(double)(current_co[first_letter]));
    } else {
      v.push_back((double)(current_to[transition] * candidate_co[first_letter])/(double)(current_co[first_letter] * candidate_to[transition]));
    }
  }

  double score = 0;
  for (double x : v) {
    score += log(x);
  }
  //cout << "Debug::" << score << endl;
  return score;
}

void learn() {
  for (string language : languages) {
    pair<unordered_map<long long, int>, unordered_map<int, int>> stat = get_statistics("learn_" + language + ".txt");
      lang_transition_occurrences[language] = stat.first;
      lang_char_occurrences[language] = stat.second;
  }
}

void test() {
  for (string language : languages) {
    pair<unordered_map<long long, int>, unordered_map<int, int>> test = get_statistics("test_" + language + ".txt");

    double max_score = -INF;
    string detected_language;

    for (string candidate : languages) {
      double score = get_score(test.first, test.second, lang_transition_occurrences[candidate], lang_char_occurrences[candidate]);
      if (score > max_score) {
        detected_language = candidate;
        max_score = score;
      }
    }

    transform(language.begin(), language.end(), language.begin(), ::toupper);
    transform(detected_language.begin(), detected_language.end(), detected_language.begin(), ::toupper);
    cout << "expected language: " << language << " ::: detected language: " << detected_language << endl;
  }
}

int main(int argc, char *argv[]) {
  learn();
  test();
  return 0;
}
