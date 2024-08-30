#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include<map>


using namespace std;

// Question class
class Question {
public:
    Question(const string& category, const string& difficulty, const string& type,
             const string& question, const string& option1, const string& option2,
             const string& option3, const string& option4, const string& answer)
        : category(category), difficulty(difficulty), type(type), question(question), answer(answer) {
        options[0] = option1;
        options[1] = option2;
        options[2] = option3;
        options[3] = option4;
    }

    string getCategory() const { return category; }
    string getDifficulty() const { return difficulty; }
    string getType() const { return type; }
    string getQuestion() const { return question; }
    string getOption(int index) const { return options[index]; }
    string getAnswer() const { return answer; }

private:
    string category;
    string difficulty;
    string type;
    string question;
    string options[4];
    string answer;
};

// Player class
class Player {
public:
    Player(const string& name, int score = 0) : name(name), score(score) {}

    string getName() const { return name; }
    int getScore() const { return score; }
    void setScore(int score) { this->score = score; }

private:
    string name;
    int score;
};

class Leaderboard {
public:
    void addScore(const Player& player, const string& mode) {
        auto& leaderboard = leaderboards[mode];
        leaderboard.push_back(player);
        sort(leaderboard.begin(), leaderboard.end(), [](const Player& a, const Player& b) {
            return a.getScore() > b.getScore();
        });
        saveToFile(mode);
    }

    void display(const string& mode) const {
        auto it = leaderboards.find(mode);
        if (it == leaderboards.end()) {
            cout << "No leaderboard for this mode!" << endl;
            return;
        }

        const auto& leaderboard = it->second;
        cout << "\n==============================\n";
        cout << "         LEADERBOARD (" << mode << ")\n";
        cout << "==============================\n";
        for (const auto& player : leaderboard) {
            cout << player.getName() << " : $" << player.getScore() << endl;
        }
        cout << "==============================\n";
    }

private:
    map<string, vector<Player>> leaderboards;

    void saveToFile(const string& mode) const {
        ofstream file(mode + "_leaderboard.txt");
        if (file.is_open()) {
            const auto& leaderboard = leaderboards.at(mode);
            for (const auto& player : leaderboard) {
                file << player.getName() << "," << player.getScore() << endl;
            }
            file.close();
        }
    }
};

class QuizGame {
public:
    void loadQuestions(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            string category, difficulty, type, question, option1, option2, option3, option4, answer;
            getline(iss, category, ',');
            getline(iss, difficulty, ',');
            getline(iss, type, ',');
            getline(iss, question, ',');
            getline(iss, option1, ',');
            getline(iss, option2, ',');
            getline(iss, option3, ',');
            getline(iss, option4, ',');
            getline(iss, answer, ',');
            questions.emplace_back(category, difficulty, type, question, option1, option2, option3, option4, answer);
        }
        file.close();
    }

    void showMainMenu() {
        cout << "\n==============================\n";
        cout << "        QUIZ GAME MAIN MENU\n";
        cout << "==============================\n";
        cout << "1. Start Standard Quiz" << endl;
        cout << "2. Multiplayer Mode" << endl;
        cout << "3. Display Leaderboard" << endl;
        cout << "4. Exit" << endl;
        cout << "5. Start Millionaire Mode" << endl;
        cout << "==============================\n";

        int choice;
        cout << "Enter your choice (1-5): ";
        cin >> choice;
        cin.ignore();  

        switch (choice) {
            case 1: startGame(); break;
            case 2: multiplayerMode(); break;
            case 3: displayLeaderboards(); break;
            case 4: exit(0); break;
            case 5: startMillionaireMode(); break;
            default: cout << "Invalid choice. Try again." << endl; showMainMenu();
        }
    }

private:
    vector<Question> questions;
    Leaderboard leaderboard;

    void startGame() {
        string name;
        cout << "\nEnter your name: ";
        getline(cin, name);
        Player player(name);

        if (questions.empty()) {
            cerr << "No questions loaded!" << endl;
            return;
        }

       
        vector<bool> asked(questions.size(), false);
        int prize = 0;
        while (true) {
         
            int index = rand() % questions.size();
            while (asked[index]) {
                index = rand() % questions.size();
            }

            asked[index] = true;
            const Question& q = questions[index];

            cout << "\nQUESTION:\n" << q.getQuestion() << endl;
            for (int i = 0; i < 4; ++i) {
                cout << char('A' + i) << ". " << q.getOption(i) << endl;
            }

            string answer;
            cout << "Your answer (A, B, C, D): ";
            getline(cin, answer);

            if (answer == q.getAnswer()) {
                cout << "Correct! Your score is $" << prize + 1000 << endl;
                prize += 1000;
            } else {
                cout << "Incorrect. The correct answer was " << q.getAnswer() << endl;
                break;
            }

            if (all_of(asked.begin(), asked.end(), [](bool v) { return v; })) {
                cout << "All questions have been asked!" << endl;
                break;
            }
        }

        player.setScore(prize);
        leaderboard.addScore(player, "Standard");
        showMainMenu(); 
    }void multiplayerMode() {
    string player1Name, player2Name;
    cout << "\nEnter Player 1's name: ";
    getline(cin, player1Name);
    Player player1(player1Name);

    cout << "Enter Player 2's name: ";
    getline(cin, player2Name);
    Player player2(player2Name);

    int player1Score = 0, player2Score = 0;

    if (questions.empty()) {
        cerr << "No questions loaded!" << endl;
        return;
    }

  
    vector<bool> asked(questions.size(), false);

    for (int i = 0; i < questions.size(); ++i) {
       
        Player& currentPlayer = (i % 2 == 0) ? player1 : player2;
        int& currentScore = (i % 2 == 0) ? player1Score : player2Score;

        cout << "\n" << currentPlayer.getName() << "'s turn!" << endl;

        
        int index = rand() % questions.size();
        while (asked[index]) {
            index = rand() % questions.size();
        }

        asked[index] = true; 
        const Question& q = questions[index];

        cout << "\nQUESTION:\n" << q.getQuestion() << endl;
        for (int j = 0; j < 4; ++j) {
            cout << char('A' + j) << ". " << q.getOption(j) << endl;
        }

        string answer;
        cout << "Your answer (A, B, C, D): ";
        getline(cin, answer);

        if (answer == q.getAnswer()) {
            cout << "Correct!" << endl;
            currentScore += 1000;
        } else {
            cout << "Incorrect. The correct answer was " << q.getAnswer() << endl;
        }

        if (all_of(asked.begin(), asked.end(), [](bool v) { return v; })) {
            cout << "All questions have been asked!" << endl;
            break;
        }
    }

    
    cout << "\nFinal Scores:\n";
    cout << player1.getName() << ": $" << player1Score << endl;
    cout << player2.getName() << ": $" << player2Score << endl;
    player1.setScore(player1Score);
    player2.setScore(player2Score);
    leaderboard.addScore(player1, "Multiplayer");
    leaderboard.addScore(player2, "Multiplayer");

    if (player1Score > player2Score) {
        cout << player1.getName() << " wins!" << endl;
    } else if (player2Score > player1Score) {
        cout << player2.getName() << " wins!" << endl;
    } else {
        cout << "It's a tie!" << endl;
    }

    showMainMenu(); 
}

void startMillionaireMode() {
    string name;
    cout << "\nEnter your name: ";
    getline(cin, name);
    Player player(name);

    if (questions.empty()) {
        cerr << "No questions loaded!" << endl;
        return;
    }

    int prize = 0;
    int level = 1;
    bool continueGame = true;

    vector<int> prizeStructure = {1000, 2000, 5000, 10000, 20000, 50000, 100000, 250000, 500000, 1000000};
    
    vector<bool> asked(questions.size(), false);

    while (continueGame && level <= 10) {
        int timeLimit = 10 + (level - 1) * 5;
        
        int index = rand() % questions.size();
        while (asked[index]) {
            index = rand() % questions.size();
        }

        asked[index] = true; 
        const Question& q = questions[index];

        cout << "\nLEVEL " << level << " QUESTION:\n" << q.getQuestion() << endl;
        for (int i = 0; i < 4; ++i) {
            cout << char('A' + i) << ". " << q.getOption(i) << endl;
        }

        cout << "You have " << timeLimit << " seconds to answer!" << endl;

        auto start = chrono::steady_clock::now();
        string answer;
        getline(cin, answer);
        auto end = chrono::steady_clock::now();

        auto elapsedSeconds = chrono::duration_cast<chrono::seconds>(end - start).count();

        if (elapsedSeconds > timeLimit) {
            cout << "Time's up! The correct answer was " << q.getAnswer() << endl;
            continueGame = false;
        } else if (answer == q.getAnswer()) {
            prize = prizeStructure[level - 1]; 
            cout << "Correct! You win $" << prize << endl;
            level++;
        } else {
            cout << "Incorrect. The correct answer was " << q.getAnswer() << endl;
            continueGame = false;
        }

        // Check if all questions have been asked
        if (all_of(asked.begin(), asked.end(), [](bool v) { return v; })) {
            cout << "All questions have been asked!" << endl;
            break;
        }
    }

    // Final prize amount will be based on the level achieved
    player.setScore(prize);
    leaderboard.addScore(player, "Millionaire");
    showMainMenu();
}

    void displayLeaderboards() {
        cout << "\nChoose the leaderboard to display:\n";
        cout << "1. Standard Quiz\n";
        cout << "2. Multiplayer Mode\n";
        cout << "3. Millionaire Mode\n";
        cout << "4. Back to Main Menu\n";

        int choice;
        cout << "Enter your choice (1-4): ";
        cin >> choice;
        cin.ignore();  

        switch (choice) {
            case 1: leaderboard.display("Standard"); break;
            case 2: leaderboard.display("Multiplayer"); break;
            case 3: leaderboard.display("Millionaire"); break;
            case 4: showMainMenu(); break;
            default: cout << "Invalid choice. Try again." << endl; displayLeaderboards();
        }
    }
};

int main() {
    srand(static_cast<unsigned int>(time(0))); 

    QuizGame game;
    game.loadQuestions("questions.txt");

    while (true) {
        game.showMainMenu();
    }

    return 0;
}
