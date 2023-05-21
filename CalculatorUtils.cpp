namespace CalculatorUtils {

    int getOperatorPrecedence(const QString& op) {
        if (op == "+" || op == "-") {
            return 1;
        }
        else if (op == "*" || op == "/") {
            return 2;
        }
        else {
            // Неизвестный оператор, можно вернуть 3 или другое значение по умолчанию
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

    // Добавляет недостающие закрывающие скобки
    void autoBalanceParentheses(QString& expression)
    {
        QStack<QChar> parenthesesStack;

        for (int i = 0; i < expression.length(); ++i) {
            QChar ch = expression.at(i);

            if (ch == '(') {
                // Открывающая скобка, добавляем ее в стек
                parenthesesStack.push(ch);
            }
            else if (ch == ')') {
                // Закрывающая скобка

                if (parenthesesStack.isEmpty()) {
                    // Нет соответствующей открывающей скобки, удаляем закрывающую скобку
                    expression.remove(i, 1);
                    --i;
                }
                else {
                    // Есть соответствующая открывающая скобка, удаляем ее из стека
                    parenthesesStack.pop();
                }
            }
        }

        // Добавляем недостающие закрывающие скобки
        while (!parenthesesStack.isEmpty()) {
            expression.append(')');
            parenthesesStack.pop();
        }
    }

    // расчитываете выражение с одним операндом
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
            // Неподдерживаемая операция
            return std::numeric_limits<double>::quiet_NaN();
        }
    }

    // перевод обычной строки в обратную польскую нотацию
    QStringList convertToRPN(const QString& expression) {
        // ...
    }

    double calculateExpression(const QString& expression) {
        // ...
    }

} // namespace CalculatorUtils
