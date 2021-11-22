#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <vector>
using namespace std;

class Strategy {
public:
	virtual int doOperation(int num1, int num2) = 0;
};

class OperationAdd : public Strategy {
public:
	int doOperation(int num1, int num2) {
		return num1 + num2;
	}
};

class OperationSubstract : public Strategy {
public:
	int doOperation(int num1, int num2) {
		return num1 - num2;
	}
};

class OperationMultiply : public Strategy {
public:
	int doOperation(int num1, int num2) {
		return num1 * num2;
	}
};

class OperationDivide : public Strategy {
public:
	int doOperation(int num1, int num2) {
		if (num2 == 0) {
			cout << "�и� 0�Դϴ�" << endl;
			exit(0);
		}
		return num1 / num2;
	}
};

class Context {
private:
	Strategy* strategy;
public:
	Context(Strategy* strategy) {
		this->strategy = strategy;
	}

	int executeStrategy(int num1, int num2) {
		return strategy->doOperation(num1, num2);
	}
};

class Number {
public:
	virtual void decimal(string& num) {};
};

class Decimal : public Number {
public:
	void decimal(string& num) {}
};

class Binary : public Number {
public:
	void decimal(string& num) {
		num = num.substr(2); // 0b����
		num = to_string(stoi(num, nullptr, 2));
	} // 2������ 10������ ����� ����
};

class Hexadecimal : public Number {
public:
	void decimal(string& num) {
		num = num.substr(2); // 0x����
		num = to_string(stoi(num, nullptr, 16));
	} // 16������ 10������ ����� ����
};

class NumberFactory {
public:
	Number* getNumber(string num) {
		// ������ ���ڰ� 1�ڸ� �̻� ����
		if (num.length() > 1) {
			if (num.at(1) == 'x')
				return new Hexadecimal();
			else if (num.at(1) == 'b')
				return new Binary();
			else
				return new Decimal();
		}
		else if (num.length() == 1)
			return new Decimal();
		else
			return NULL;
	}
};

// �ٷ� ���� �����ȣ ������ Ȯ��
bool Operator_symbol(char pre_c) {
	if (pre_c == '*' || pre_c == '/' || pre_c == '+' || pre_c == '-')
		return true;
	else
		return false;
}

void Exception() {
	cout << "�߸��� ���� �Է��߽��ϴ�." << endl;
	exit(0);
}

// ���� ������ �����
void Formula(string formula, string& change, char pre_c, vector<string>& n) {
	stack<char> s1;
	string num = ""; // �ʱ�ȭ
	for (char c : formula) {
		if (c == '(') {
			// '(' �տ� (, �����ȣ, �� ó�� ����
			if (Operator_symbol(pre_c) == false && pre_c != ' ' && pre_c != '(')
				Exception();
			s1.push(c);
		}
		else if (c == ')') {
			// ')' �տ� ����, ���ĺ�, ) ����
			if (pre_c == '(' || Operator_symbol(pre_c) == true)
				Exception();
			// 0x, 0b�� �Է�
			if (num.length() == 2)
				if (pre_c == 'x' || pre_c == 'b')
					Exception();
			NumberFactory* numberFactory = new NumberFactory();
			Number* number = numberFactory->getNumber(num);
			if (number != NULL) {
				number->decimal(num);
				change += num;
				n.push_back(num);
				num = "";
			}
			while (s1.empty() == false && s1.top() != '(') {
				change += s1.top();
				s1.pop();
			}
			if (s1.empty() == true) // while�� ���鼭 '('�Է��� ã�� ����
				Exception();
			s1.pop(); // '(' ���ÿ��� ����
		}
		else if (c == '*' || c == '/' || c == '+' || c == '-') {
			// �����ȣ�տ� ����, ���ĺ�, ) ����
			if (pre_c == ' ' || pre_c == '(' || Operator_symbol(pre_c) == true)
				Exception();
			// 0x, 0b�� �Է�
			if (num.length() == 2)
				if (pre_c == 'x' || pre_c == 'b')
					Exception();
			NumberFactory* numberFactory = new NumberFactory();
			Number* number = numberFactory->getNumber(num);
			if (number != NULL) {
				number->decimal(num);
				change += num;
				n.push_back(num);
				num = ""; // �ʱ�ȭ
			}
			if (s1.empty() == false) {
				if (c == '+' || c == '-') {
					while (s1.top() == '+' || s1.top() == '-' || s1.top() == '*' || s1.top() == '/') {
						change += s1.top();
						s1.pop();
						if (s1.empty() == true)
							break;
					}
				}
				else {
					while (s1.top() == '*' || s1.top() == '/') {
						change += s1.top();
						s1.pop();
						if (s1.empty() == true)
							break;
					}
				}
			}
			s1.push(c);
		}
		else if ((c >= 48 && c <= 57)) {
			num += c;
			// 0b�ڿ� 0�� 1�� ����
			if (num.length() > 2 && num.at(1) == 'b') {
				if (!(c == '0' || c == '1'))
					Exception();
			}
			// ������ �Է� 0���� ���� �Ұ�
			else if (num.length() >= 2) {
				if (!(num.at(1) == 'b' || num.at(1) == 'x') && num.at(0) == '0')
					Exception();
			}
		}
		else if (c == 'x' || (c >= 97 && c <= 102)) {
			num += c;
			if (num.length() == 2) {
				// 0x �Ǵ� 0b (������ ���ĺ����� �ȵ�)
				if (!(c == 'x' || c == 'b'))
					Exception();
				// �� ���� 0
				else {
					if (pre_c != '0')
						Exception();
				}
			}
			// x�� 3��°���� �Ұ���, 0x�� �ƴϸ� �Ұ���
			else if (num.length() > 2) {
				if (c == 'x' || num.at(1) != 'x')
					Exception();
			}
			// �� �� �Ұ���
			else
				Exception();
		}
		// (, ), �����ȣ, ����, a~f, x�� ����
		else
			Exception();
		pre_c = c;
	}
	NumberFactory* numberFactory = new NumberFactory();
	Number* number = numberFactory->getNumber(num);
	if (number != NULL) {
		number->decimal(num);
		change += num;
		n.push_back(num);
		num = "";
	}
	while (s1.empty() == false) {
		// () ���� �� �̷�
		if (s1.top() == '(')
			Exception();
		change += s1.top();
		s1.pop();
	}
	cout << "���� ������ : " << change << endl;
}

int calculation(string change, vector<string> n) {
	stack<int> s2;
	vector<string>::iterator iter = n.begin();
	string num = "";
	// ���� ������ ���
	for (char c : change) {
		int cal;
		if (c == '+' || c == '-' || c == '*' || c == '/') {
			int a = s2.top();
			s2.pop();
			int b = s2.top();
			s2.pop();
			if (c == '+') {
				Context* context = new Context(new OperationAdd());
				cal = context->executeStrategy(a, b);
			}
			else if (c == '-') {
				Context* context = new Context(new OperationSubstract());
				cal = context->executeStrategy(b, a);
			}
			else if (c == '*') {
				Context* context = new Context(new OperationMultiply());
				cal = context->executeStrategy(a, b);
			}
			else {
				Context* context = new Context(new OperationDivide());
				cal = context->executeStrategy(b, a);
			}
			s2.push(cal);
		}
		else {
			num.push_back(c);
			if (*iter == num) {
				s2.push(stoi(num));
				num = ""; // �ʱ�ȭ
				++iter;
			} // �Է��� ���̶� ����
		}
	}
	int result = s2.top();
	s2.pop();
	return result;
}

int main() {
	string formula = "";
	string change = "";
	char pre_c = ' ';
	cout << "�Է� �� : ";
	getline(cin, formula);
	vector<string> num; // �� ���� ����
	if (!((formula.back() >= 48 && formula.back() <= 57) || (formula.back() >= 97 && formula.back() <= 102)
		|| (formula.back() == ')')))
		Exception();
	if (formula.length() > 0) {
		Formula(formula, change, pre_c, num);
		cout << "��� �� : " << calculation(change, num) << endl;
	}
	return 0;
}