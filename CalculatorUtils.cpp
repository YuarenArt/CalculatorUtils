namespace CalculatorUtils {

    int getOperatorPrecedence(const QString& op) {
        if (op == "+" || op == "-") {
            return 1;
        }
        else if (op == "*" || op == "/") {
            return 2;
        }
        else {
            // ����������� ��������, ����� ������� 3 ��� ������ �������� �� ���������
            return 3;
        }
    }

    bool isValidInput(const QString& input)
    {
        QRegularExpression validRegex("^[0-9()+\\-*/. ]+$");
        QRegularExpressionMatch match = validRegex.match(input);
        return match.hasMatch();
    }

    bool isOperator(const QString& token)
    {
        return token == "+" || token == "-" || token == "*" || token == "/";
    }

    // ��������� ����������� ����������� ������
    void autoBalanceParentheses(QString& expression)
    {
        QStack<QChar> parenthesesStack;

        for (int i = 0; i < expression.length(); ++i) {
            QChar ch = expression.at(i);

            if (ch == '(') {
                // ����������� ������, ��������� �� � ����
                parenthesesStack.push(ch);
            }
            else if (ch == ')') {
                // ����������� ������

                if (parenthesesStack.isEmpty()) {
                    // ��� ��������������� ����������� ������, ������� ����������� ������
                    expression.remove(i, 1);
                    --i;
                }
                else {
                    // ���� ��������������� ����������� ������, ������� �� �� �����
                    parenthesesStack.pop();
                }
            }
        }

        // ��������� ����������� ����������� ������
        while (!parenthesesStack.isEmpty()) {
            expression.append(')');
            parenthesesStack.pop();
        }
    }

    // ������������ ��������� � ����� ���������
    double performOperation(double operand1, double operand2, const QString& operation)
    {
        if (operation == "+") {
            return operand1 + operand2;
        }
        else if (operation == "-") {
            return operand1 - operand2;
        }
        else if (operation == "*") {
            return operand1 * operand2;
        }
        else if (operation == "/") {
            if (operand2 == 0) {
                QMessageBox::warning(nullptr, "Warning", "You divide by zero");
            }
            return operand1 / operand2;
        }
        else {
            // ���������������� ��������
            return std::numeric_limits<double>::quiet_NaN();
        }
    }

    // ������� ������� ������ � �������� �������� �������
    QStringList convertToRPN(const QString& expression) {
        // ...
    }

    double calculateExpression(const QString& expression) {
        // ...
    }

} // namespace CalculatorUtils
