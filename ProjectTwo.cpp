/*
    Jaden Williams
    CS300 - Project Two
    2/20/2025
*/

// I intitially believed a Hash Table would be the best fit for this project, but upon getting into the programming I found the BinarySearchTree to be the better method so I implemented it instead. 

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace std;

const int GLOBAL_SLEEP_TIME = 5000; // Default sleep time for pause in execution

// Struct to hold course information
struct Course {
    string courseId;
    string courseName;
    vector<string> preList;
};

class BinarySearchTree {
private:
    struct Node {
        Course course;
        Node* left;
        Node* right;

        Node(Course aCourse) : course(aCourse), left(nullptr), right(nullptr) {}
    };

    Node* root;
    int size;

    void inOrder(Node* node);

public:
    BinarySearchTree();
    void InOrder();
    void Insert(Course aCourse);
    void Remove(string courseId);
    Course Search(string courseId);
    int Size();
};

// Default constructor
BinarySearchTree::BinarySearchTree() : root(nullptr), size(0) {}

// In-order traversal of the tree
void BinarySearchTree::InOrder() {
    inOrder(root);
}

// Recursive helper function for in-order traversal
void BinarySearchTree::inOrder(Node* node) {
    if (node != nullptr) {
        inOrder(node->left);
        cout << node->course.courseId << ", " << node->course.courseName << endl;
        inOrder(node->right);
    }
}

// Insert a course into the binary search tree
void BinarySearchTree::Insert(Course aCourse) {
    Node* currentNode = root;
    if (root == nullptr) {
        root = new Node(aCourse);
    }
    else {
        while (currentNode != nullptr) {
            if (aCourse.courseId < currentNode->course.courseId) {
                if (currentNode->left == nullptr) {
                    currentNode->left = new Node(aCourse);
                    currentNode = nullptr;
                }
                else {
                    currentNode = currentNode->left;
                }
            }
            else {
                if (currentNode->right == nullptr) {
                    currentNode->right = new Node(aCourse);
                    currentNode = nullptr;
                }
                else {
                    currentNode = currentNode->right;
                }
            }
        }
    }
    size++;
}

// Remove a course from the binary search tree
void BinarySearchTree::Remove(string courseId) {
    Node* par = nullptr;
    Node* curr = root;

    while (curr != nullptr) {
        if (curr->course.courseId == courseId) {
            if (curr->left == nullptr && curr->right == nullptr) {
                if (par == nullptr) {
                    root = nullptr;
                }
                else if (par->left == curr) {
                    par->left = nullptr;
                }
                else {
                    par->right = nullptr;
                }
            }
            else if (curr->right == nullptr) {
                if (par == nullptr) {
                    root = curr->left;
                }
                else if (par->left == curr) {
                    par->left = curr->left;
                }
                else {
                    par->right = curr->left;
                }
            }
            else if (curr->left == nullptr) {
                if (par == nullptr) {
                    root = curr->right;
                }
                else if (par->left == curr) {
                    par->left = curr->right;
                }
                else {
                    par->right = curr->right;
                }
            }
            else {
                Node* suc = curr->right;
                while (suc->left != nullptr) {
                    suc = suc->left;
                }
                Node successorData = Node(suc->course);
                Remove(suc->course.courseId);
                curr->course = successorData.course;
            }
            return;
        }
        else if (curr->course.courseId < courseId) {
            par = curr;
            curr = curr->right;
        }
        else {
            par = curr;
            curr = curr->left;
        }
    }
    cout << "\nCourse ID not found." << endl;
}

// Search for a course by its ID
Course BinarySearchTree::Search(string courseId) {
    Node* currentNode = root;
    while (currentNode != nullptr) {
        if (currentNode->course.courseId == courseId) {
            return currentNode->course;
        }
        else if (courseId < currentNode->course.courseId) {
            currentNode = currentNode->left;
        }
        else {
            currentNode = currentNode->right;
        }
    }
    return Course();  // Return an empty course if not found
}

// Get the total number of courses in the tree
int BinarySearchTree::Size() {
    return size;
}

// Split a string by a delimiter (comma in this case)
vector<string> Split(string lineFeed) {
    vector<string> lineTokens;
    string temp = "";
    for (char ch : lineFeed) {
        if (ch == ',') {
            lineTokens.push_back(temp);
            temp = "";
        }
        else {
            temp += ch;
        }
    }
    if (!temp.empty()) {
        lineTokens.push_back(temp);
    }
    return lineTokens;
}

// Load courses from a CSV file
void loadCourses(string csvPath, BinarySearchTree* courseList) {
    ifstream inFS(csvPath);
    if (!inFS.is_open()) {
        cout << "Could not open file. Please check the file path." << endl;
        return;
    }

    string line;
    while (getline(inFS, line)) {
        vector<string> stringTokens = Split(line);
        if (stringTokens.size() < 2) {
            cout << "Error in line format. Skipping line: " << line << endl;
            continue;
        }

        Course aCourse;
        aCourse.courseId = stringTokens[0];
        aCourse.courseName = stringTokens[1];

        for (size_t i = 2; i < stringTokens.size(); i++) {
            aCourse.preList.push_back(stringTokens[i]);
        }

        courseList->Insert(aCourse);
    }
    inFS.close();
}

// Display course information with prerequisites
void displayCourse(const Course& aCourse) {
    cout << aCourse.courseId << ", " << aCourse.courseName << endl;
    cout << "Prerequisites: ";
    if (aCourse.preList.empty()) {
        cout << "None" << endl;
    }
    else {
        for (size_t i = 0; i < aCourse.preList.size(); ++i) {
            cout << aCourse.preList[i];
            if (i < aCourse.preList.size() - 1) cout << ", ";
        }
        cout << endl;
    }
}

// Convert a string to uppercase
void convertCase(string& toConvert) {
    transform(toConvert.begin(), toConvert.end(), toConvert.begin(), ::toupper);
}

int main(int argc, char* argv[]) {
    string csvPath = "courselist.csv";  // Default file path
    if (argc == 2) {
        csvPath = argv[1];  // Override with user-provided file path
    }

    BinarySearchTree* courseList = new BinarySearchTree();
    int choice = 0;
    string courseKey;
    Course foundCourse; // Declare foundCourse outside the switch, this was a big issue for no reason

    while (choice != 9) {
        cout << "\nMenu:" << endl;
        cout << "  1. Load Courses" << endl;
        cout << "  2. Display All Courses" << endl;
        cout << "  3. Find Course" << endl;
        cout << "  4. Remove Course" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            loadCourses(csvPath, courseList);
            cout << courseList->Size() << " courses loaded." << endl;
            break;

        case 2:
            cout << "Courses in the Computer Science Department (sorted):" << endl;
            courseList->InOrder();
            break;

        case 3:
            cout << "Enter course ID to find: ";
            cin >> courseKey;
            convertCase(courseKey);
            foundCourse = courseList->Search(courseKey); // Assign foundCourse here
            if (!foundCourse.courseId.empty()) {
                displayCourse(foundCourse);
            }
            else {
                cout << "Course not found!" << endl;
            }
            break;

        case 4:
            cout << "Enter course ID to remove: ";
            cin >> courseKey;
            convertCase(courseKey);
            courseList->Remove(courseKey);
            break;

        case 9:
            cout << "Exiting program." << endl;
            exit(0);

        default:
            cout << "Invalid choice. Please try again." << endl;
            break;
        }

        // Clear input buffer before next loop
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    delete courseList;
    return 0;
}

