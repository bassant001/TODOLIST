#include<iostream>
#include<string>
#include<fstream>
#include<ctime>
#include<conio.h>
using namespace std;

bool ChangeUserData = 0;	//the value will change to 1 if a new user signed up or if a user changed their username or password
bool ChangeTaskData = 0;	//the value will change to 1 if the user adds, edits, marks or deletes a specific task
const char Userdel = ',';	//delimiter for users file
const char Taskdel = '-';	//delimiter for tasks file
int usersize = 1;

time_t now; //used to represent time
struct tm nowlocal; // hold date in form easier to understand than time_t

void today_date()
{
	now = time(nullptr); // Get time from OS, we are not storing result in time_t,
	// It also returns the current time (as a time_t value) without storing it in a separate variable
#ifdef _WIN32 // used in windwos
	localtime_s(&nowlocal, &now);  //it takes &now to time_t and convert it to local time, stoting result in now local

#else // used in unix system like mac os and linux
	localtime_r(&now, &nowlocal);
#endif
}
/*
both versions (WIN32 AND unix system) convert the time_t value to local time and store it in the nowlocal structure
*/

struct date
{
	int day, month, year;
};

struct task
{
	int number;
	string title;
	string details;
	int priority;
	date due_date;
	bool isdone;
};

struct user
{
	int tasksize = 1;
	int TaskCount = 0;	//TaskCount is the number of tasks entered by the user
	string username;
	string password;
	task* tasks = new task[tasksize];
	//task tasks[20];
}*users = new user[usersize]; //*users = new user[usersize];users[100];

void LoadUserData(int& UserCount);			//transfer user data from file to array
void LoadUsersTaskData(int UserCount);		// transfer tasks data from file to array
void AccMenu();
void Acc2Menu();
void login(int UserCount, user* CurrentUser, string choice);
void SaveUserData(int UserCount);		//save user data from array to file (if any change happens)
void SignUp(int& UserCount);
string TaskMenu();
void SaveUserTaskData(int UserCount);	//transfer tasks data from array to file (if any change happens)
void done_tasks(user* CurrentUser);
void overdue_tasks(user* CurrentUser);
void displayall(user* CurrentUser);
void display_search(int i, user* CurrentUser);
void search(user* CurrentUser);
void display_today_task(user* CurrentUser);
void delete_task(user* CurrentUser);
void edit_task(user* CurrentUser);
void change_name_pass(user* CurrentUser, int UserCount);
void delete_account(user* CurrentUser, int& UserCount);
void addTask(user* CurrentUser);
void markTaskDone(user* CurrentUser);
void swap(int& n1, int& n2);
void sort(user* CurrentUser);
void resize_task(task*& arr, int&);
void resize_user(user*& arr);

int main()
{
	ofstream UsersFile("Users.txt", ios::app);
	ofstream TasksFile("Tasks.txt", ios::app);
	int UserCount = 0;	//number of registered users
	string UserAccChoice;
	LoadUserData(UserCount);
	user* CurrentUser = 0;
	cout << "\t\t\t\t================ Welcome to To-Do list ================" << endl;
	do
	{
		AccMenu();
		cin >> UserAccChoice;

		if (UserAccChoice == "1")
		{
			resize_user(users);
			SignUp(UserCount);
		}

		else if (UserAccChoice == "2")
		{
			string choice;
			login(UserCount, CurrentUser, choice);
		}

		else if (UserAccChoice == "3")
		{
			cout << "Exiting program..." << endl;
		}

		else
			cout << "Invalid choice. Please try again.\n";

	} while (UserAccChoice != "3");

	if (ChangeUserData == 1)
		SaveUserData(UserCount);
	if (ChangeTaskData == 1)
		SaveUserTaskData(UserCount);

	delete[] users;
	return 0;
}

void LoadUsersTaskData(int UserCount)
{
	ifstream TasksFile("Tasks.txt");
	string line, TaskComponent;
	int i = 0;
	if (TasksFile.is_open())
	{
		while (getline(TasksFile, line) && i < UserCount)
		{
			if (line == "END")
			{
				i++;
				continue;
			}
			int pos = 0;
			string LineComponents[8];	//to separate number, title, details, priority, day, month, year and isdone boolean of a task
			int index = 0;
			while (pos != -1)
			{
				pos = line.find(Taskdel);	//Taskdel = -
				TaskComponent = line.substr(0, pos);
				LineComponents[index] = TaskComponent;
				index++;
				line = line.substr(pos + 1);
			}

			int j = users[i].TaskCount;
			users[i].tasks[j].number = stoi(LineComponents[0]);
			users[i].tasks[j].title = LineComponents[1];
			users[i].tasks[j].details = LineComponents[2];
			users[i].tasks[j].priority = stoi(LineComponents[3]);
			users[i].tasks[j].due_date.day = stoi(LineComponents[4]);
			users[i].tasks[j].due_date.month = stoi(LineComponents[5]);
			users[i].tasks[j].due_date.year = stoi(LineComponents[6]);
			users[i].tasks[j].isdone = stoi(LineComponents[7]);
			users[i].TaskCount++;
			resize_task(users[i].tasks, users[i].tasksize);
		}
	}
	else
	{
		cout << "ERROR: Failed to load task data." << endl;
	}
	TasksFile.close();
}

void LoadUserData(int& UserCount)
{
	ifstream UsersFile("Users.txt");
	string line, component;
	UserCount = 0;
	if (UsersFile.is_open())
	{
		while (getline(UsersFile, line))
		{
			int pos = 0;
			string LineComponents[2];	//to separate username and password
			int index = 0;
			while (pos != -1)
			{
				pos = line.find(Userdel);	//Userdel = , pos=-1
				component = line.substr(0, pos);
				LineComponents[index] = component;
				index++;
				line = line.substr(pos + 1);
			}
			users[UserCount].username = LineComponents[0];
			users[UserCount].password = LineComponents[1];
			UserCount++;
			resize_user(users);
		}
	}

	else
	{
		cout << "ERROR: Failed to load user data." << endl;
		return;
	}
	UsersFile.close();

	LoadUsersTaskData(UserCount);
}

void AccMenu()
{
	cout << "Menu\n" << "-----\n";
	cout << "1. Sign Up" << endl;
	cout << "2. Login" << endl;
	cout << "3. Exit" << endl;
	cout << "Enter your choice: ";
}

void Acc2Menu()
{
	cout << "\nMenu\n" << "-----\n";
	cout << "Press\n";
	cout << "1. To add a new task\n";
	cout << "2. To mark a task as done\n";
	cout << "3. To edit a specific task\n";
	cout << "4. To delete a specific task\n";
	cout << "5. To display all tasks\n";
	cout << "6. To display all done tasks\n";
	cout << "7. To display all overdue tasks\n";
	cout << "8. To display tasks that need to be done today\n";
	cout << "9. To search for a specific task by (title or number or timing)\n";
	cout << "10. To change your username or password\n";
	cout << "11. Sort tasks\n";
	cout << "12. To delete your account\n";
	cout << "13. Logout\n";
	cout << "choice: ";
}

string TaskMenu()
{
	string choice;
	while (true)
	{
		Acc2Menu();
		cin >> choice;
		if (choice == "1" || choice == "2" || choice == "3" || choice == "4" || choice == "5" || choice == "6" || choice == "7" || choice == "8" || choice == "9" || choice == "10" || choice == "11" || choice == "12" || choice == "13")
		{
			return choice;
			break;
		}
		else
		{
			cout << "Invalid choice. Please try again.\n";
			cout << "---------------------------------\n";
		}
	}
}

void login(int UserCount, user* CurrentUser, string choice)
{
	string enteredUsername;
	cout << "\nLogin\n";
	cout << "------\n";
	cout << "Enter username: ";
	cin >> enteredUsername;
	char enteredPassword[100], ch;
	int i = 0;
	cout << "Enter password: ";
	while ((ch = _getch()) != '\r')
	{
		if (ch == '\b' && i > 0)
		{
			cout << "\b \b";
			i--;
		}
		else if (ch != '\b')
		{
			cout << '*';
			enteredPassword[i++] = ch;
		}
	}
	enteredPassword[i] = '\0';

	for (int i = 0; i < UserCount; i++)
	{
		if (users[i].username == enteredUsername && users[i].password == enteredPassword)
		{
			CurrentUser = &users[i];
			cout << "\n\nLogin successful!\n";
			cout << "Welcome! " << CurrentUser->username << "\n";
			do
			{
				choice = TaskMenu();
				cout << "\n";
				if (choice == "1")
				{
					resize_task(users[i].tasks, CurrentUser->tasksize);
					addTask(CurrentUser);
				}

				else if (choice == "2")
				{
					displayall(CurrentUser);
					markTaskDone(CurrentUser);
				}

				else if (choice == "3")
				{
					displayall(CurrentUser);
					edit_task(CurrentUser);

				}

				else if (choice == "4")
				{
					displayall(CurrentUser);
					delete_task(CurrentUser);
				}

				else if (choice == "5")
				{
					displayall(CurrentUser);
				}

				else if (choice == "6")
				{
					done_tasks(CurrentUser);
				}

				else if (choice == "7")
				{
					overdue_tasks(CurrentUser);
				}

				else if (choice == "8")
				{
					display_today_task(CurrentUser);
				}

				else if (choice == "9")
				{
					search(CurrentUser);
				}

				else if (choice == "10")
				{
					change_name_pass(CurrentUser, UserCount);
				}

				else if (choice == "11")
				{
					sort(CurrentUser);
				}

				else if (choice == "12")
				{
					delete_account(CurrentUser, UserCount);
					return;
				}

				else if (choice == "13")
				{
					cout << "Thank you for using to do list!\nLogging out...\n\n";
					return;
				}

				else
				{
					cout << "Invalid choice!. Please try again.\n";
					continue;
				}

			} while (true);		//Until the user decides to logout

		}
	}

	if (CurrentUser == 0)	//if the user entered an invalid username or password (the pointer CurrentUser won't find an address to point to)
	{
		cout << "\n\nInvalid username or password. Please try again.\n";
		cout << "-----------------------------------------------\n\n";
		return;
	}
}

void SaveUserData(int UserCount)
{
	ofstream UsersFile("Users.txt", ios::trunc);	//trunc is used to delete old data if file and replace it by the new one (in case a new user is added)
	if (UsersFile.is_open())
	{
		for (int i = 0; i < UserCount; i++)
		{
			if (users[i].username != "0")
			{
				UsersFile << users[i].username << Userdel << users[i].password << endl;
			}
		}
	}
	else
	{
		cout << "ERROR: Failed to save user data." << endl;
		return;
	}
	UsersFile.close();
}

void SignUp(int& UserCount)
{
	bool UsernameExists = 0;
	user newUser;

	cout << "\nRegistration\n";
	cout << "-------------\n";
	cout << "Create a username: ";
	cin >> newUser.username;     //storing username in newuser.username
	cin.ignore();
	for (int i = 0; i < UserCount; i++)
	{
		if (users[i].username == newUser.username)	//if the new user created a username that already existed
		{
			UsernameExists = 1;
			break;
		}
	}
	if (UsernameExists == 1)
	{
		cout << "The username you entered already exists. Please create a new one." << endl;
		SignUp(UserCount);
	}
	else
	{
		cout << "Create a password: ";
		cin >> newUser.password;     //storing password in newuser.password
		cin.ignore();

		users[UserCount] = newUser;   //the new user struct is added to the users array where (users[userCount]=newUser)
		UserCount++;                  //usercount plus one

		cout << "\nRegistration successful! You can login now.\n";
		cout << "--------------------------------------------\n\n";
		ChangeUserData = 1;
	}
}

void SaveUserTaskData(int UserCount)
{
	ofstream TasksFile("Tasks.txt", ios::trunc);	//trunc is used to replace old tasks data by the new one
	if (TasksFile.is_open())
	{
		for (int i = 0; i < UserCount; i++)
		{
			if (users[i].username != "0")
			{
				for (int j = 0; j < users[i].TaskCount; j++)
				{
					TasksFile << users[i].tasks[j].number << Taskdel
						<< users[i].tasks[j].title << Taskdel
						<< users[i].tasks[j].details << Taskdel
						<< users[i].tasks[j].priority << Taskdel
						<< users[i].tasks[j].due_date.day << Taskdel
						<< users[i].tasks[j].due_date.month << Taskdel
						<< users[i].tasks[j].due_date.year << Taskdel
						<< users[i].tasks[j].isdone << endl;
				}
				TasksFile << "END" << endl;
			}
		}
	}
	else
	{
		cout << "ERROR: Failed to save task data." << endl;
		return;
	}
	TasksFile.close();
}
void addTask(user* CurrentUser)
{
	task newTask;
	newTask.number = CurrentUser->TaskCount + 1;

	cout << "Enter task title: ";
	cin.ignore();
	getline(cin, newTask.title);

	cout << "Enter task details: ";
	getline(cin, newTask.details);

	cout << "Enter task priority (1-Low, 2-Medium, 3-High): ";
	cin >> newTask.priority;
	while (cin.fail() || newTask.priority < 1 || newTask.priority > 3)
	{
		cin.clear();
		cin.ignore(10000, '\n');
		cout << "Invalid input." << endl;
		cout << "Enter task priority (1-Low, 2-Medium, 3-High): ";
		cin >> newTask.priority;
	}

	cout << "Enter task due date" << endl;
	cout << "Day: ";
	cin >> newTask.due_date.day;
	while (cin.fail() || newTask.due_date.day < 1 || newTask.due_date.day > 31)
	{
		cin.clear();
		cin.ignore(10000, '\n');
		cout << "Invalid input." << endl;
		cout << "Day: ";
		cin >> newTask.due_date.day;
	}
	cout << "Month: ";
	cin >> newTask.due_date.month;
	while (cin.fail() || newTask.due_date.month < 1 || newTask.due_date.month > 12)
	{
		cin.clear();
		cin.ignore(10000, '\n');
		cout << "Invalid input." << endl;
		cout << "Month: ";
		cin >> newTask.due_date.month;
	}
	cout << "Year: ";
	cin >> newTask.due_date.year;
	while (cin.fail() || newTask.due_date.year < 1)
	{
		cin.clear();
		cin.ignore(10000, '\n');
		cout << "Invalid input." << endl;
		cout << "Year: ";
		cin >> newTask.due_date.year;
	}

	newTask.isdone = false;

	CurrentUser->tasks[CurrentUser->TaskCount] = newTask;
	CurrentUser->TaskCount++;

	cout << "Task added successfully.\n";
	cout << "------------------------\n";
	ChangeTaskData = 1;
}
void markTaskDone(user* CurrentUser)
{
	if (CurrentUser->TaskCount == 0)
	{
		cout << "There are no tasks.\n";
		cout << "-------------------\n";
		return;
	}
	int tasIndex;
	cout << "Enter the number of the task that you want to mark as done: ";
	cin >> tasIndex;
	while (cin.fail())
	{
		cin.clear();
		cin.ignore(10000, '\n');
		cout << "Invalid input." << endl;
		cout << "Enter the number of the task that you want to mark as done: ";
		cin >> tasIndex;
	}
	if (tasIndex <= CurrentUser->TaskCount && tasIndex > 0)
	{
		if (CurrentUser->tasks[tasIndex - 1].isdone)
		{
			cout << "\nThis task is marked as done before.\n" << endl;
			cout << "-----------------------------------\n";
		}
		else
		{
			CurrentUser->tasks[tasIndex - 1].isdone = true;
			cout << "\nTask is marked as done.\n";
			cout << "-----------------------\n";
			ChangeTaskData = 1;
		}
	}

	else
	{
		cout << "\nInvalid task number\n";
		cout << "-------------------\n";
	}
}

void displayall(user* CurrentUser)
{
	cout << "All tasks:\n" << "---------\n";

	if (CurrentUser->TaskCount == 0)
	{
		cout << "Empty\n------\n";
	}
	else
	{
		for (int i = 0; i < CurrentUser->TaskCount; i++)
		{

			cout << "\nTask " << i + 1 << ": \n" << "------\n";

			cout << CurrentUser->tasks[i].title << "\t";

			cout << "(Due: " << CurrentUser->tasks[i].due_date.day << "/" << CurrentUser->tasks[i].due_date.month << "/" << CurrentUser->tasks[i].due_date.year << "\t";

			if (CurrentUser->tasks[i].priority == 1)
				cout << "Priority: Low)\n";
			else if (CurrentUser->tasks[i].priority == 2)
				cout << "Priority: Medium)\n";
			else if (CurrentUser->tasks[i].priority == 3)
				cout << "Priority: High)\n";

			cout << CurrentUser->tasks[i].details << "\n";
			cout << "---------------------------------\n";

		}
	}

	return;
}

void display_search(int i, user* CurrentUser) {
	cout << "\nTask " << i + 1 << ": \n" << "------\n";

	cout << CurrentUser->tasks[i].title << "\t";

	cout << "(Due: " << CurrentUser->tasks[i].due_date.day << "/" << CurrentUser->tasks[i].due_date.month << "/" << CurrentUser->tasks[i].due_date.year << "\t";

	if (CurrentUser->tasks[i].priority == 1)
		cout << "Priority: Low)\n";
	else if (CurrentUser->tasks[i].priority == 2)
		cout << "Priority: Medium)\n";
	else if (CurrentUser->tasks[i].priority == 3)
		cout << "Priority: High)\n";

	cout << CurrentUser->tasks[i].details << "\n";
	cout << "---------------------------------\n";

	return;
}

void search(user* CurrentUser)
{
	bool found = false;
	cout << "Do you want to search by (title - timing - number) ?\n";
	string search_choice;
	cin >> search_choice;
	if (search_choice == "title")
	{
		cout << "What title do you search ?\n";
		string search_title;
		cin >> search_title;
		for (int i = 0; i < CurrentUser->TaskCount; i++)
		{
			if (CurrentUser->tasks[i].title == search_title)
			{
				found = true;
				if (found)
				{
					display_search(i, CurrentUser);
				}
			}
		}
		if (!found)
		{
			cout << "Not Found\n";
			cout << "----------\n";
		}
	}
	else if (search_choice == "timing")
	{
		cout << "By (day - month - year - all)?\n";
		string search_timing;
		cin >> search_timing;
		if (search_timing == "all")
		{
			cout << "What history do you search ?\n";
			int search_day, search_month, search_year;
			cin >> search_day >> search_month >> search_year;
			for (int i = 0; i < CurrentUser->TaskCount; i++)
			{
				if (CurrentUser->tasks[i].due_date.day == search_day && CurrentUser->tasks[i].due_date.month == search_month && CurrentUser->tasks[i].due_date.year == search_year)
				{
					found = true;
					if (found)
					{
						display_search(i, CurrentUser);
					}
				}
			}
			if (!found)
			{
				cout << "Not Found\n";
				cout << "----------\n";
			}
		}
		else if (search_timing == "year")
		{
			cout << "What year do you search ?\n";
			int search_year;
			cin >> search_year;
			for (int i = 0; i < CurrentUser->TaskCount; i++)
			{
				if (CurrentUser->tasks[i].due_date.year == search_year)
				{
					found = true;
					if (found)
					{
						display_search(i, CurrentUser);
					}
				}
			}
			if (!found)
			{
				cout << "Not Found\n";
				cout << "----------\n";
			}
		}
		else if (search_timing == "month")
		{
			cout << "What month do you search ?\n";
			int search_month;
			cin >> search_month;
			for (int i = 0; i < CurrentUser->TaskCount; i++)
			{
				if (CurrentUser->tasks[i].due_date.month == search_month)
				{
					found = true;
					if (found)
					{
						display_search(i, CurrentUser);
					}
				}
			}
			if (!found)
			{
				cout << "Not Found\n";
				cout << "----------\n";
			}
		}
		else if (search_timing == "day")
		{
			cout << "What day do you search ?\n";
			int search_day;
			cin >> search_day;
			for (int i = 0; i < CurrentUser->TaskCount; i++)
			{
				if (CurrentUser->tasks[i].due_date.day == search_day)
				{
					found = true;
					if (found)
					{
						display_search(i, CurrentUser);
					}
				}
			}
			if (!found)
			{
				cout << "Not Found\n";
				cout << "----------\n";
			}
		}
	}
	else if (search_choice == "number")
	{
		cout << "What number do you search ?\n";
		int search_number;
		cin >> search_number;
		for (int i = 0; i < CurrentUser->TaskCount; i++)
		{
			if (CurrentUser->tasks[i].number == search_number)
			{
				found = true;
				if (found)
				{
					display_search(i, CurrentUser);
				}
			}
		}
		if (!found)
		{
			cout << "Not Found\n";
			cout << "----------\n";
		}
	}
	else
	{
		cout << "Invalid Input.\n";
		cout << "--------------\n";
	}
}

void done_tasks(user* CurrentUser)
{
	bool empty = true;
	cout << "Done tasks: \n";
	cout << "-----------\n";

	for (int index = 0; index < (*CurrentUser).TaskCount; index++)
	{
		if (CurrentUser->tasks[index].isdone == true)
		{
			cout << "Task " << index + 1 << "\n-----\n";
			cout << CurrentUser->tasks[index].title << "\t";
			cout << "(Due: " << CurrentUser->tasks[index].due_date.day << "/" << CurrentUser->tasks[index].due_date.month << "/" << CurrentUser->tasks[index].due_date.year << "\t";
			if (CurrentUser->tasks[index].priority == 1)
				cout << "Priority: Low)\n";
			else if (CurrentUser->tasks[index].priority == 2)
				cout << "Priority: Medium)\n";
			else if (CurrentUser->tasks[index].priority == 3)
				cout << "Priority: High)\n";
			cout << CurrentUser->tasks[index].details << "\n";
			cout << "---------------------------------\n";
			empty = false;
		}
	}
	if (empty)
	{
		cout << "There is no done tasks.\n ";
		cout << "-----------------------\n";
	}
}

void overdue_tasks(user* CurrentUser)
{
	bool empty = true;
	today_date();
	cout << "Overdue tasks: \n";
	cout << "--------------\n";

	for (int index = 0; index < (*CurrentUser).TaskCount; index++)
	{
		if ((CurrentUser->tasks[index].due_date.year < nowlocal.tm_year + 1900) || (CurrentUser->tasks[index].due_date.year == nowlocal.tm_year + 1900 && CurrentUser->tasks[index].due_date.month < nowlocal.tm_mon + 1) || (CurrentUser->tasks[index].due_date.year == nowlocal.tm_year + 1900 && CurrentUser->tasks[index].due_date.month == nowlocal.tm_mon + 1 && CurrentUser->tasks[index].due_date.day < nowlocal.tm_mday))
		{
			if (CurrentUser->tasks[index].isdone == false)
			{
				cout << "Task " << index + 1 << "\n-----\n";
				cout << CurrentUser->tasks[index].title << "\t\t";
				cout << "(Due: " << CurrentUser->tasks[index].due_date.day << "/" << CurrentUser->tasks[index].due_date.month << "/" << CurrentUser->tasks[index].due_date.year << "\t";
				if (CurrentUser->tasks[index].priority == 1)
					cout << "Priority: Low)\n";
				else if (CurrentUser->tasks[index].priority == 2)
					cout << "Priority: Medium)\n";
				else if (CurrentUser->tasks[index].priority == 3)
					cout << "Priority: High)\n";
				cout << CurrentUser->tasks[index].details << "\n";
				cout << "---------------------------------\n";
				empty = false;
			}
		}
	}
	if (empty)
	{
		cout << "There is no overdue tasks.\n";
		cout << "--------------------------\n";
	}
}
void display_today_task(user* CurrentUser)
{
	today_date();
	cout << "Tasks that need to be done today:\n";
	cout << "--------------------------------\n";
	bool Empty = true;
	for (int i = 0; i < CurrentUser->TaskCount; i++)
	{
		if (CurrentUser->tasks[i].due_date.day == nowlocal.tm_mday && CurrentUser->tasks[i].due_date.month == nowlocal.tm_mon + 1 && CurrentUser->tasks[i].due_date.year == nowlocal.tm_year + 1900 && CurrentUser->tasks[i].isdone == false)
		{
			if (CurrentUser->tasks[i].isdone == false)
			{
				Empty = false;
				cout << "Task " << i + 1 << "\n-----\n";
				cout << CurrentUser->tasks[i].title << "\t\t";
				cout << "(Due: " << CurrentUser->tasks[i].due_date.day << "/" << CurrentUser->tasks[i].due_date.month << "/" << CurrentUser->tasks[i].due_date.year << "\t";
				if (CurrentUser->tasks[i].priority == 1)
					cout << "Priority: Low)\n";
				else if (CurrentUser->tasks[i].priority == 2)
					cout << "Priority: Medium)\n";
				else if (CurrentUser->tasks[i].priority == 3)
					cout << "Priority: High)\n";
				cout << CurrentUser->tasks[i].details << endl;
				cout << "---------------------------------\n";
			}
		}
	}
	if (Empty)
	{
		cout << "There are no tasks that need to be done today.\n";
		cout << "----------------------------------------------\n";
	}

}

void delete_task(user* CurrentUser)
{
	if (CurrentUser->TaskCount == 0)
	{
		cout << "There are no tasks.\n";
		cout << "-------------------\n";
		return;
	}
	int delete_choice;
	cout << "Enter the number of the task you want to delete: ";
	cin >> delete_choice;

	if (delete_choice < 1 || delete_choice > CurrentUser->TaskCount)
	{
		cout << "Invalid task number. Please try again.\n";
		cout << "--------------------------------------\n";
		return;
	}

	// Shift tasks to fill the deleted task
	for (int i = delete_choice - 1; i < CurrentUser->TaskCount - 1; i++)
	{
		CurrentUser->tasks[i] = CurrentUser->tasks[i + 1];
	}

	CurrentUser->TaskCount--; // Decrease the task count
	cout << "\nTask deleted successfully.\n";
	cout << "--------------------------\n";
	ChangeTaskData = 1;
}

void edit_task(user* CurrentUser)
{
	int task_index;
	string choice1;
	string choice2;

	if (CurrentUser->TaskCount == 0)
	{
		cout << "There are no tasks.\n";
		cout << "-------------------\n";
		return;
	}

	cout << "Enter task number to edit: ";
	cin >> task_index;
	while (cin.fail())
	{
		cin.clear();
		cin.ignore(10000, '\n');
		cout << "Invalid input." << endl;
		cout << "Enter task number to edit: ";
		cin >> task_index;
	}
	if (task_index <= CurrentUser->TaskCount && task_index > 0)
	{
		do {
			cout << "Press\n-----\n";
			cout << "1. To edit title" << endl;
			cout << "2. To edit details" << endl;
			cout << "3. To edit due date" << endl;
			cout << "4. To edit priority" << endl;
			cout << "Enter your choice: ";
			cin >> choice1;
			cin.ignore();

			if (choice1 == "1")
			{
				cout << "Enter new task title: ";
				getline(cin, CurrentUser->tasks[task_index - 1].title);
			}
			else if (choice1 == "2")
			{
				cout << "Enter new task details: ";
				getline(cin, CurrentUser->tasks[task_index - 1].details);
			}
			else if (choice1 == "3")
			{
				cout << "Enter new due date" << endl;
				cout << "Day: ";
				cin >> CurrentUser->tasks[task_index - 1].due_date.day;
				while (cin.fail() || CurrentUser->tasks[task_index - 1].due_date.day < 1 || CurrentUser->tasks[task_index - 1].due_date.day > 31)
				{
					cin.clear();
					cin.ignore(10000, '\n');
					cout << "Invalid input. Please try again." << endl;
					cout << "Day :";
					cin >> CurrentUser->tasks[task_index - 1].due_date.day;
				}

				cout << "Month :";
				cin >> CurrentUser->tasks[task_index - 1].due_date.month;
				while (cin.fail() || CurrentUser->tasks[task_index - 1].due_date.month < 1 || CurrentUser->tasks[task_index - 1].due_date.month > 12)
				{
					cin.clear();
					cin.ignore(10000, '\n');
					cout << "Invalid input. Please try again." << endl;
					cout << "Month: ";
					cin >> CurrentUser->tasks[task_index - 1].due_date.month;
				}

				cout << "Year: ";
				cin >> CurrentUser->tasks[task_index - 1].due_date.year;
				while (cin.fail() || CurrentUser->tasks[task_index - 1].due_date.year < 1)
				{
					cin.clear();
					cin.ignore(10000, '\n');
					cout << "Invalid input. Please try again." << endl;
					cout << "Year: ";
					cin >> CurrentUser->tasks[task_index - 1].due_date.year;
				}
			}
			else if (choice1 == "4")
			{
				cout << "Enter new priority (1-Low, 2-Medium, 3-High): ";
				cin >> CurrentUser->tasks[task_index - 1].priority;
				while (cin.fail() || CurrentUser->tasks[task_index - 1].priority < 1 || CurrentUser->tasks[task_index - 1].priority > 3)
				{
					cin.clear();
					cin.ignore(10000, '\n');
					cout << "Invalid input." << endl;
					cout << "Enter new priority (1-Low, 2-Medium, 3-High): ";
					cin >> CurrentUser->tasks[task_index - 1].priority;
				}
			}
			else
			{
				cout << "Invalid choice.\n";
				cout << "---------------\n";
			}

			cout << "Do you want to do it again? (y/n): ";
			cin >> choice2;
		} while (choice2 == "y" || choice2 == "Y");

		cout << "\nTask edited successfully.\n";
		cout << "-------------------------\n";
		ChangeTaskData = 1;
	}
	else
	{
		cout << "Invalid task number\n";
		cout << "-------------------\n";
	}
}

void change_name_pass(user* CurrentUser, int UserCount)
{
	cout << "Press:\n-----\n";
	cout << "1. To change username\n";
	cout << "2. To change password\n";
	cout << "3. To change username and password\n";
	string choice, name, pass;
	bool UsernameExists = 0;
	cin >> choice;
	if (choice == "1")
	{
		cout << "Write the new username: ";
		cin >> name;
		for (int i = 0; i < UserCount; i++)
		{
			if (users[i].username == name)	//if the new user created a username that already existed
			{
				UsernameExists = 1;
				break;
			}
		}
		if (UsernameExists == 1)
		{
			cout << "The username you entered already exists. Please create a new one." << endl;
			change_name_pass(CurrentUser, UserCount);
		}
		CurrentUser->username = name;
		cout << "Username is changed successfully\n";
		cout << "--------------------------------\n";
	}
	else if (choice == "2")
	{
		cout << "Write the new password: ";
		cin >> pass;
		CurrentUser->password = pass;
		cout << "Password is changed successfully\n";
		cout << "--------------------------------\n";

	}
	else if (choice == "3")
	{
		cout << "Write the new username: ";
		cin >> name;
		for (int i = 0; i < UserCount; i++)
		{
			if (users[i].username == name)	//if the new user created a username that already existed
			{
				UsernameExists = 1;
				break;
			}
		}

		if (UsernameExists == 1)
		{
			cout << "The username you entered already exists. Please create a new one." << endl;
			change_name_pass(CurrentUser, UserCount);
		}

		CurrentUser->username = name;
		cout << "Write the new password: ";
		cin >> pass;
		CurrentUser->password = pass;
		cout << "Username and password are changed successfully\n";
		cout << "----------------------------------------------\n";

	}
	else
	{
		cout << "Invalid input\n";
		cout << "-------------\n";
	}
}

void delete_account(user* CurrentUser, int& UserCount)
{
	CurrentUser->username = "0";
	int i;
	for (i = 0; i < UserCount; i++)
	{
		if (users[i].username == "0")
			break;
	}
	while (i < UserCount - 1)
	{
		users[i] = users[i + 1];
		i++;
	}     // 0 1 2 3 4

	users[UserCount - 1].username = "0";
	users[UserCount - 1].password = "0";
	users[UserCount - 1].TaskCount = 0;
	UserCount--;

	cout << "Account is deleted successfully.\n";
	cout << "--------------------------------\n\n";
	ChangeUserData = 1;
	ChangeTaskData = 1;
	return;
}

// to be user friendly 
void swap(int& n1, int& n2)
{
	int n3 = n1;
	n1 = n2;
	n2 = n3;
}

void sort(user* CurrentUser)
{
	while (true)
	{
		cout << "Do you want to sort by: ( 1-due date from oldest to newest or 2-due date from newest to oldest  or 3-proirty h->l )\n";
		char answer;
		cin >> answer;
		if (answer == '1')
		{
			for (int i = 0; i < CurrentUser->TaskCount - 1; i++)
			{
				for (int j = 0; j < CurrentUser->TaskCount - 1 - i; j++)
				{
					if ((CurrentUser->tasks[j].due_date.year > CurrentUser->tasks[j + 1].due_date.year) || (CurrentUser->tasks[j].due_date.year == CurrentUser->tasks[j + 1].due_date.year && CurrentUser->tasks[j].due_date.month > CurrentUser->tasks[j + 1].due_date.month) || (CurrentUser->tasks[j].due_date.year == CurrentUser->tasks[j + 1].due_date.year && CurrentUser->tasks[j].due_date.month == CurrentUser->tasks[j + 1].due_date.month && CurrentUser->tasks[j].due_date.day > CurrentUser->tasks[j + 1].due_date.day))
					{
						swap(CurrentUser->tasks[j], CurrentUser->tasks[j + 1]);
						swap(CurrentUser->tasks[j].number, CurrentUser->tasks[j + 1].number); // num is cont index
					}
				}
			}
			cout << "DONE\n----\n";
			break;
		}

		if (answer == '2')
		{
			for (int i = 0; i < CurrentUser->TaskCount - 1; i++)
			{
				for (int j = 0; j < CurrentUser->TaskCount - 1 - i; j++)
				{
					if ((CurrentUser->tasks[j].due_date.year < CurrentUser->tasks[j + 1].due_date.year) || (CurrentUser->tasks[j].due_date.year == CurrentUser->tasks[j + 1].due_date.year && CurrentUser->tasks[j].due_date.month < CurrentUser->tasks[j + 1].due_date.month) || (CurrentUser->tasks[j].due_date.year == CurrentUser->tasks[j + 1].due_date.year && CurrentUser->tasks[j].due_date.month == CurrentUser->tasks[j + 1].due_date.month && CurrentUser->tasks[j].due_date.day < CurrentUser->tasks[j + 1].due_date.day))
					{
						swap(CurrentUser->tasks[j], CurrentUser->tasks[j + 1]);
						swap(CurrentUser->tasks[j].number, CurrentUser->tasks[j + 1].number); // num is cont index
					}
				}
			}
			cout << "DONE\n----\n";
			break;
		}
		else if (answer == '3') // h>>l
		{
			for (int i = 0; i < CurrentUser->TaskCount - 1; i++)
			{
				for (int j = 0; j < CurrentUser->TaskCount - 1 - i; j++)
				{
					if (CurrentUser->tasks[j].priority < CurrentUser->tasks[j + 1].priority && CurrentUser->tasks[j].priority < CurrentUser->tasks[j + 1].priority && CurrentUser->tasks[j].priority < CurrentUser->tasks[j + 1].priority)
					{
						swap(CurrentUser->tasks[j], CurrentUser->tasks[j + 1]);
						swap(CurrentUser->tasks[j].number, CurrentUser->tasks[j + 1].number); // num is cont index
					}
				}
			}
			cout << "DONE\n----\n";
			break;
		}
		else
		{
			cout << "Invalid choice. Please try again.\n";
			cout << "---------------------------------\n";
			continue;
		}
	}
}

void resize_task(task*& arr, int& tasksize)
{
	tasksize++;
	task* arr1 = new task[tasksize];
	for (int i = 0; i < tasksize - 1; i++)
	{
		arr1[i] = arr[i];
	}
	delete[] arr;
	arr = arr1;
	arr1 = NULL;
}

void resize_user(user*& arr)
{
	usersize++;
	user* arr1 = new user[usersize];
	for (int i = 0; i < usersize - 1; i++)
	{
		arr1[i] = arr[i];
	}
	delete[] arr;
	arr = arr1;
	arr1 = NULL;
}

//try git hup