#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <cstdlib>
#include <ctime>
using namespace std;

// =====================================================
//  GLOBAL GAME STATE
// =====================================================
bool spicy = false;
bool isGirl = false;

enum Stat { HAPPINESS, PARENTS_KHUSHI, LOG_KYA_SOCHENGE, STAT_COUNT };
int stats[STAT_COUNT] = {100, 0, 0};          // ARRAY
string personality = "Not formed yet";

// =====================================================
//  TREE (linked nodes) + VECTOR
//  Each SceneNode holds a vector of Choices.
//  Each Choice points to a child SceneNode.
// =====================================================
struct SceneNode;

struct Choice {
    string label;             // text shown in the menu
    string result;            // text shown after choosing
    int newStats[STAT_COUNT]; // -1 = leave unchanged
    string personality;       // "" = leave unchanged
    SceneNode* next;          // child node (nullptr = end of story)
};

struct SceneNode {
    int age;
    string title;
    string preface;           // optional screen shown before the scene
    string text;
    int statsShown;           // how many stats to show after the choice
    vector<Choice> choices;   // VECTOR
};

// =====================================================
//  HELPERS
// =====================================================
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

int getChoice(int low, int high) {
    int c;
    while (true) {
        cout << "\nEnter choice: ";
        if (cin >> c && c >= low && c <= high) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return c;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid choice. Try again.\n";
    }
}

void showTitle() {
    cout << "=====================================\n";
    cout << "   NPC : DIGITAL LIFE GAME\n";
    cout << "        INDIAN VERSION\n";
    cout << "=====================================\n\n";
}

void showStats(int count) {
    cout << "\nYOUR STATS:\n";
    cout << "HAPPINESS         : " << stats[HAPPINESS] << "\n";
    if (count >= 2)
        cout << "PARENTS KI KHUSHI : " << stats[PARENTS_KHUSHI] << "\n";
    if (count >= 3)
        cout << "LOG KYA SOCHENGE  : " << stats[LOG_KYA_SOCHENGE] << "\n";
}

// =====================================================
//  TREE BUILDING HELPERS
// =====================================================
SceneNode* newNode(int age, const string& title, const string& preface,
                   const string& text, int statsShown) {
    SceneNode* n = new SceneNode;
    n->age = age;
    n->title = title;
    n->preface = preface;
    n->text = text;
    n->statsShown = statsShown;
    return n;
}

Choice makeChoice(const string& label, const string& result,
                  int h, int p, int l, const string& pers, SceneNode* next) {
    Choice c;
    c.label = label;
    c.result = result;
    c.newStats[HAPPINESS] = h;
    c.newStats[PARENTS_KHUSHI] = p;
    c.newStats[LOG_KYA_SOCHENGE] = l;
    c.personality = pers;
    c.next = next;
    return c;
}

void addChoice(SceneNode* node, const Choice& c) {
    node->choices.push_back(c);
}

// Recursive delete. Works because this is a real tree
// (every node has exactly one parent).
void freeTree(SceneNode* n) {
    if (n == nullptr) return;
    for (size_t i = 0; i < n->choices.size(); i++) {
        freeTree(n->choices[i].next);
    }
    delete n;
}

// =====================================================
//  THE STORY TREE
//
//  BIRTH (root)
//   |-- MALE   -> AGE 10 (boy)
//   |              |-- enjoy friends -> AGE 13 (boy, fun)   -> 2 endings
//   |              |-- studies       -> AGE 13 (boy, study) -> 2 endings
//   |-- FEMALE -> AGE 10 (girl)
//                  |-- studies       -> AGE 13 (girl, study) -> 2 endings
//                  |-- friends       -> AGE 13 (girl, fun)   -> 2 endings
// =====================================================
SceneNode* buildTree() {
    const string boyPreface =
        "Your height has been growing like crazy.\n"
        "Facial hair has started showing up.\n"
        "And your voice has deepened.\n";

    const string girlPreface =
        "* Your periods have started, and you hate the cramps and\n"
        "  pain that come with them.\n"
        "* Your brother keeps getting taller and getting praised,\n"
        "  while you get told to sit properly and dress appropriately.\n"
        "* You've started noticing the partiality at home, and sometimes\n"
        "  it feels like even God is playing favourites.\n"
        "* You realise how much easier being a man seems to be.\n"
        "* Some men have suddenly become more helpful and polite.\n"
        "  That's nice, at least.\n"
        "* But some get weird when you call them bhaiya and tell you\n"
        "  to stop.\n"
        "* Men in groups have started staring at you for a little too\n"
        "  long, making you uncomfortable.\n";

    // ---------- create all nodes ----------
    SceneNode* root = newNode(0, "BIRTH", "", "Choose your gender.", 1);

    SceneNode* boy10 = newNode(10, "PEAK CHILDHOOD", "",
        "It's your peak childhood.\n"
        "How do you want to live this phase?\n"
        "SPOILER: this will be the base of your whole personality.", 2);

    SceneNode* girl10 = newNode(10, "PEAK CHILDHOOD", "",
        "It's your peak childhood.\n"
        "Your mother has started asking you for help with household\n"
        "work, even when your brother is just watching TV.\n"
        "Never mind, it's just simple work.\n\n"
        "How do you want to live this phase?\n"
        "SPOILER: this will be the base of your whole personality.", 2);

    SceneNode* boyStudy13 = newNode(13, "PUBERTY: YOU TRANSFORM", boyPreface,
        "You are the class topper now, but you have become even more\n"
        "competitive about marks, and you feel more pressure to\n"
        "succeed academically.\n\n"
        "You see some of your classmates enjoying life and having\n"
        "a great group of friends.", 3);

    SceneNode* boyFun13 = newNode(13, "PUBERTY: YOU TRANSFORM", boyPreface,
        "You have a very great friend group. You are confident, funny,\n"
        "and everyone around you likes you. Teachers know your name,\n"
        "and you never have to eat lunch alone.\n\n"
        "But you have started to feel a lot of pressure to be liked\n"
        "by everyone. You can't handle being disliked by anyone.", 3);

    SceneNode* girlStudy13 = newNode(13, "PUBERTY: YOU TRANSFORM", girlPreface,
        "You're one of the brightest students in your class. Your\n"
        "teachers expect great things from you, and your parents have\n"
        "started thinking about your future.\n\n"
        "But puberty has brought its own set of problems. You're told\n"
        "to be careful about what you wear, who you talk to, and how\n"
        "you behave. Meanwhile, your classmates are making friends and\n"
        "enjoying their teenage years.", 3);

    SceneNode* girlFun13 = newNode(13, "PUBERTY: YOU TRANSFORM", girlPreface,
        "You have a great group of friends. You laugh a lot, make\n"
        "memories, and actually look forward to going to school.\n\n"
        "You're confident, expressive, and comfortable around people.\n"
        "You know the latest gossip, have plenty of stories to tell,\n"
        "and rarely spend your entire lunch break discussing homework.\n\n"
        "But puberty has changed things. Your parents have started\n"
        "asking more questions about where you're going, who's coming\n"
        "along, and whether any boys will be there.", 3);

    // ---------- root: birth ----------
    addChoice(root, makeChoice("MALE",   "", -1, -1, -1, "", boy10));
    addChoice(root, makeChoice("FEMALE", "", -1, -1, -1, "", girl10));

    // ---------- boy, age 10 ----------
    addChoice(boy10, makeChoice(
        "Enjoy with your friends more",
        "Wow, you really enjoyed your childhood. You are very happy.\n"
        "But your parents are not, and neither is your teacher.\n"
        "You get compared with Sharma ji ka ladka every day.\n",
        90, 60, -1, "", boyFun13));

    addChoice(boy10, makeChoice(
        "Focus on studies more",
        "Really great, you have become Sharma ji ka ladka.\n"
        "Maybe some classmates don't like you, but your teacher\n"
        "definitely does. You got 90/100 in 4th std, but your\n"
        "parents still ask why you didn't get those 10.\n",
        75, 90, -1, "", boyStudy13));

    // ---------- girl, age 10 ----------
    addChoice(girl10, makeChoice(
        "Prioritize studies",
        "Good. Your mother tells every relative that you are a\n"
        "very polite and smart girl.\n"
        "\"Meri beti ko ghar ka kaam bhi aata hai aur padhai bhi\n"
        "kar leti hai.\"\n",
        70, 85, -1, "", girlStudy13));

    addChoice(girl10, makeChoice(
        "Prioritize some friends",
        "Your mother tells you to focus on studies more, or they\n"
        "will marry you off soon. Obviously as a joke, but still,\n"
        "you are so young.\n",
        80, 75, -1, "", girlFun13));

    // ---------- boy, age 13, study path ----------
    addChoice(boyStudy13, makeChoice(
        "Open up to people and explore your changing identity",
        "Your marks start to decline, but your parents are kind of\n"
        "happy that you finally have some friends. Your teacher is\n"
        "not. She keeps telling you that your focus is decreasing\n"
        "and that you should not hang out with those students.\n"
        "Somehow, other people have started to judge you too.\n",
        85, 85, 70, "More expressive, socially confident, emotionally aware",
        nullptr));

    addChoice(boyStudy13, makeChoice(
        "Focus entirely on studies. You have a goal to achieve",
        "Your parents adore you, but they tell you to at least go\n"
        "out once a day. They have already started planning whether\n"
        "you will give JEE, NEET or UPSC.\n"
        "You have barely any friends, and your classmates sometimes\n"
        "make fun of you. But you love PTMs, because other parents\n"
        "always come to you for advice.\n",
        70, 90, 95, "Disciplined, goal-driven, respected, but lonely at times",
        nullptr));

    // ---------- boy, age 13, fun path ----------
    addChoice(boyFun13, makeChoice(
        "Become the funny guy in your friend group",
        "You make jokes, tease your friends, and know how to make\n"
        "everyone laugh. People enjoy having you around.\n"
        "But sometimes you feel like you always have to be funny,\n"
        "even when you're not in the mood.\n"
        "Other people like your confidence, but your parents don't.\n",
        85, 80, 85, "Funny, confident, popular, but hides moods behind jokes",
        nullptr));

    addChoice(boyFun13, makeChoice(
        "Become the guy who's always up for fun",
        "You hang out with friends, try new things, and make the\n"
        "most of your teenage years. You always have something going on.\n"
        "But sometimes you agree to things you don't want to do,\n"
        "just because you don't want to miss out.\n"
        "Your parents tell you not to be with your friends so much,\n"
        "or you will pick up bad habits. Every late night ends with\n"
        "a lecture, and every new friend gets a background check.\n",
        80, 70, 80, "Adventurous, spontaneous, but struggles to say no",
        nullptr));

    // ---------- girl, age 13, study path ----------
    addChoice(girlStudy13, makeChoice(
        "Start making time for yourself and your friends",
        "You begin hanging out with classmates, talking about things\n"
        "other than exams, and enjoying life outside the classroom.\n"
        "Your marks aren't as perfect as before, but you're happier\n"
        "having people to share your life with.\n\n"
        "Your parents are glad you've made friends, but they still\n"
        "remind you not to lose focus. Some teachers think you're\n"
        "becoming distracted, and relatives have started asking why\n"
        "you aren't studying as much as you used to.\n",
        85, 85, 75, "More outgoing, socially confident, emotionally aware",
        nullptr));

    addChoice(girlStudy13, makeChoice(
        "Focus entirely on your studies. You have a goal to achieve",
        "You keep working hard, and your parents couldn't be prouder.\n"
        "They've started discussing your future career and whether\n"
        "you should prepare for JEE, NEET, or UPSC someday.\n\n"
        "You love PTMs because your teachers praise you, and other\n"
        "parents ask yours how they raised such a good student.\n\n"
        "But you have very few friends, and some classmates think\n"
        "you're too serious. At home, you're also expected to help\n"
        "out because apparently, being a topper doesn't excuse you\n"
        "from household chores.\n",
        70, 95, 90,
        "Disciplined, ambitious, independent, but prone to putting pressure on yourself",
        nullptr));

    // ---------- girl, age 13, fun path ----------
    addChoice(girlFun13, makeChoice(
        "Become the funny girl in your friend group",
        "You make jokes, tease your friends, and know how to make\n"
        "everyone laugh. People enjoy having you around, and you're\n"
        "often the one who brings everyone together when there's drama.\n\n"
        "But sometimes you feel like you always have to be cheerful,\n"
        "even when something is bothering you. People expect you to\n"
        "laugh things off, and some relatives think you're becoming\n"
        "too loud or outspoken.\n\n"
        "Your friends love your confidence, but your parents keep\n"
        "telling you to behave more seriously and stop acting like\n"
        "you're still a little kid.\n",
        85, 80, 85,
        "Funny, expressive, socially confident, but sometimes hides her feelings behind humour",
        nullptr));

    addChoice(girlFun13, makeChoice(
        "Make the most of your teenage years",
        "You spend time with friends, try new things, and make\n"
        "memories instead of worrying about every little thing. You\n"
        "enjoy going out, discovering new interests, and getting to\n"
        "know people outside your usual circle.\n\n"
        "But your parents worry that you're spending too much time\n"
        "outside and that your friends might influence you badly.\n"
        "Every outing comes with questions, every late return leads\n"
        "to an argument, and every male friend seems to require an\n"
        "explanation.\n\n"
        "Your friends think you're fun to be around, but at home,\n"
        "you're constantly reminded that a girl's reputation matters\n"
        "and that you should know your limits.\n",
        90, 70, 75,
        "Adventurous, independent, open-minded, but sometimes struggles with family expectations",
        nullptr));

    return root;
}

// =====================================================
//  MENU SCREENS
// =====================================================
void whatIsThis() {
    clearScreen();
    showTitle();
    cout << "WHAT IS THIS GAME?\n\n";
    cout << "This is a basic choice-based game set in India.\n";
    cout << "You are born, you make choices, and life happens.\n";
    cout << "It is nothing serious. Just fun.\n";
    pause();
}

void howToPlay() {
    clearScreen();
    showTitle();
    cout << "HOW TO PLAY\n\n";
    cout << "Just type the number of the choice you want.\n";
    cout << "Like in real life, you have the freedom of choice.\n";
    cout << "(Whether the choice matters is another question.)\n";
    pause();
}

void settings() {
    clearScreen();
    showTitle();
    cout << "SETTINGS\n\n";
    cout << "Show spicy facts and stats?\n\n";
    cout << "1. YES (spicy ON)\n";
    cout << "2. NO  (spicy OFF)\n";

    int c = getChoice(1, 2);
    if (c == 1) {
        spicy = true;
        cout << "\nSpicy is ON.\n";
    } else {
        spicy = false;
        cout << "\nSpicy is OFF.\n";
    }
    pause();
}

// =====================================================
//  BIRTH: returns 0 (male) or 1 (female) = index into root->choices
// =====================================================
int chooseGender(SceneNode* root) {
    clearScreen();
    showTitle();
    cout << "CHOOSE YOUR GENDER:\n\n";
    for (size_t i = 0; i < root->choices.size(); i++) {
        cout << (i + 1) << ". " << root->choices[i].label << "\n";
    }

    int c = getChoice(1, 2);
    clearScreen();

    if (c == 1) {
        isGirl = false;
        int sisters = 2 + rand() % 3;
        cout << "Congratulations, you're a boy!\n\n";
        cout << "Your entire family is overjoyed. You have "
             << sisters << " older sisters, but don't worry,\n";
        cout << "they barely count. Your parents finally got what they wanted:\n";
        cout << "someone to carry the family name, inherit the property,\n";
        cout << "and prove the previous attempts weren't in vain.\n";

        if (spicy) {
            cout << "\nSPICE: In India, around 35% of families with two daughters\n";
            cout << "and no sons wanted another child, compared with 9% of\n";
            cout << "families with two sons and no daughters. [NFHS-5]\n";
        }
        pause();
    } else {
        isGirl = true;
        cout << "Sorry, you spawned in the wrong family.\n\n";
        cout << "Your existence became a problem the moment\n";
        cout << "they discovered you weren't a boy.\n";

        if (spicy) {
            cout << "\nSPICE: An estimated 9 million girls went missing due to\n";
            cout << "sex-selective abortions between 2000 and 2019.\n";
        }
        cout << "\nPress Enter to try again...";
        cin.get();

        clearScreen();
        cout << "Congratulations! This time you were born into a good family,\n";
        cout << "and they all love you.\n";
        pause();
    }
    return c - 1;
}

void ageZero() {
    clearScreen();
    showTitle();
    cout << "AGE 0: You are born.\n";
    showStats(1);
    pause();
}

// =====================================================
//  GENERIC SCENE ENGINE - plays ANY node of the tree
//  Returns the child node the player walked into
//  (nullptr = the story has ended).
// =====================================================
void showHeader(const SceneNode* n) {
    clearScreen();
    showTitle();
    cout << "AGE " << n->age << " : " << n->title << "\n\n";
}

SceneNode* playScene(SceneNode* node) {
    showHeader(node);

    if (!node->preface.empty()) {
        cout << node->preface;
        pause();
        showHeader(node);
    }

    cout << node->text << "\n\n";

    for (size_t i = 0; i < node->choices.size(); i++) {
        cout << (i + 1) << ". " << node->choices[i].label << "\n";
    }

    int pick = getChoice(1, (int)node->choices.size());
    Choice& c = node->choices[pick - 1];

    clearScreen();
    cout << c.result;

    for (int i = 0; i < STAT_COUNT; i++) {
        if (c.newStats[i] != -1) stats[i] = c.newStats[i];
    }
    if (!c.personality.empty()) personality = c.personality;

    showStats(node->statsShown);
    if (!c.personality.empty()) cout << "\nPERSONALITY: " << personality << "\n";
    pause();

    return c.next;
}

// =====================================================
//  MAIN
// =====================================================
int main() {
    srand((unsigned)time(0));

    SceneNode* root = buildTree();

    while (true) {
        clearScreen();
        showTitle();
        cout << "CHOOSE ONE OPTION:\n\n";
        cout << "1. WHAT IS THIS GAME\n";
        cout << "2. HOW TO PLAY\n";
        cout << "3. SETTINGS\n";
        cout << "4. START\n";

        int c = getChoice(1, 4);

        if (c == 1)      whatIsThis();
        else if (c == 2) howToPlay();
        else if (c == 3) settings();
        else break;
    }

    int g = chooseGender(root);
    ageZero();

    // walk down the tree until we reach a leaf
    SceneNode* current = root->choices[g].next;
    while (current != nullptr) {
        current = playScene(current);
    }

    clearScreen();
    showTitle();
    cout << "To be continued...\n";

    freeTree(root);
    return 0;
}
