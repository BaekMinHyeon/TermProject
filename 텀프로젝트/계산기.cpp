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
			cout << "분모가 0입니다" << endl;
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
		num = num.substr(2); // 0b제거
		num = to_string(stoi(num, nullptr, 2));
	} // 2진수를 10진수로 만들어 저장
};

class Hexadecimal : public Number {
public:
	void decimal(string& num) {
		num = num.substr(2); // 0x제거
		num = to_string(stoi(num, nullptr, 16));
	} // 16진수를 10진수로 만들어 저장
};

class NumberFactory {
public:
	Number* getNumber(string num) {
		// 무조건 숫자가 1자리 이상 나옴
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

// 바로 앞이 연산기호 였는지 확인
bool Operator_symbol(char pre_c) {
	if (pre_c == '*' || pre_c == '/' || pre_c == '+' || pre_c == '-')
		return true;
	else
		return false;
}

void Exception() {
	cout << "잘못된 식을 입력했습니다." << endl;
	exit(0);
}

// 후위 연산자 만들기
void Formula(string formula, string& change, char pre_c, vector<string>& n) {
	stack<char> s1;
	string num = ""; // 초기화
	for (char c : formula) {
		if (c == '(') {
			// '(' 앞에 (, 연산기호, 맨 처음 가능
			if (Operator_symbol(pre_c) == false && pre_c != ' ' && pre_c != '(')
				Exception();
			s1.push(c);
		}
		else if (c == ')') {
			// ')' 앞에 숫자, 알파벳, ) 가능
			if (pre_c == '(' || Operator_symbol(pre_c) == true)
				Exception();
			// 0x, 0b만 입력
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
			if (s1.empty() == true) // while을 돌면서 '('입력을 찾지 못함
				Exception();
			s1.pop(); // '(' 스택에서 빼냄
		}
		else if (c == '*' || c == '/' || c == '+' || c == '-') {
			// 연산기호앞에 숫자, 알파벳, ) 가능
			if (pre_c == ' ' || pre_c == '(' || Operator_symbol(pre_c) == true)
				Exception();
			// 0x, 0b만 입력
			if (num.length() == 2)
				if (pre_c == 'x' || pre_c == 'b')
					Exception();
			NumberFactory* numberFactory = new NumberFactory();
			Number* number = numberFactory->getNumber(num);
			if (number != NULL) {
				number->decimal(num);
				change += num;
				n.push_back(num);
				num = ""; // 초기화
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
			// 0b뒤에 0과 1만 가능
			if (num.length() > 2 && num.at(1) == 'b') {
				if (!(c == '0' || c == '1'))
					Exception();
			}
			// 십진수 입력 0으로 시작 불가
			else if (num.length() >= 2) {
				if (!(num.at(1) == 'b' || num.at(1) == 'x') && num.at(0) == '0')
					Exception();
			}
		}
		else if (c == 'x' || (c >= 97 && c <= 102)) {
			num += c;
			if (num.length() == 2) {
				// 0x 또는 0b (나머지 알파벳들은 안됨)
				if (!(c == 'x' || c == 'b'))
					Exception();
				// 맨 앞이 0
				else {
					if (pre_c != '0')
						Exception();
				}
			}
			// x는 3번째부터 불가능, 0x가 아니면 불가능
			else if (num.length() > 2) {
				if (c == 'x' || num.at(1) != 'x')
					Exception();
			}
			// 맨 앞 불가능
			else
				Exception();
		}
		// (, ), 연산기호, 숫자, a~f, x만 가능
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
		// () 쌍을 안 이룸
		if (s1.top() == '(')
			Exception();
		change += s1.top();
		s1.pop();
	}
	cout << "후위 연산자 : " << change << endl;
}

int calculation(string change, vector<string> n) {
	stack<int> s2;
	vector<string>::iterator iter = n.begin();
	string num = "";
	// 후위 연산자 계산
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
				num = ""; // 초기화
				++iter;
			} // 입력한 값이랑 같음
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
	cout << "입력 식 : ";
	getline(cin, formula);
	vector<string> num; // 식 숫자 저장
	if (!((formula.back() >= 48 && formula.back() <= 57) || (formula.back() >= 97 && formula.back() <= 102)
		|| (formula.back() == ')')))
		Exception();
	if (formula.length() > 0) {
		Formula(formula, change, pre_c, num);
		cout << "결과 값 : " << calculation(change, num) << endl;
	}
	return 0;
}