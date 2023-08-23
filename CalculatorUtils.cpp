#include <QRegularExpression>
#include <QMessageBox>
#include <QStack>
#include <QJSEngine>
#include <stack>

//пространство имен
namespace CalculatorUtils {

    // Шаблон для операндов, которые могут использоваться в выражении.
    // Содержит символы [+\\-*/.^lgsqrtloglnsincostancottgctg].
    // Знак минус в методе isOperator он принимает отрицательное число за операнд
    // Поэтому проверка минуса и отрицательного числа вынесена как отдельная функция isNegativeNumber
    const QString OPERANDS = "[+\\-*/^lgsqrtloglnsincostancottgctg]";

    // Шаблон для проверки правильности всех символов в текущем выражении.
    // Может содержать только цифры, скобки, арифметические операторы и функции.
    const QString ALLCURRENTSIMBOL = "^[0-9()+\\-*/.^lgsqrtloglnsincostancottgctg ]+$";

    // Шаблон для операндов, требующих только один аргумент.
    // Включает функции [lgsqrtloglnsincostancottgctg].
    const QString OPERANDSRWITHONEARG = "[lgsqrtloglnsincostancottgctg]";

    // Вовхрващает значение приоритета операции - чем выше приоритет, тем раньше она выполняется
    int getOperatorPrecedence(const QString& op) {
        if (op == "^" || op == "sqrt" || op == "log" || op == "ln" || op == "sin" || op == "cos" || op == "tan" || op == "cot" || op == "tg" || op == "ctg") {
            return 4;
        }
        else if (op == "*" || op == "/") {
            return 3;
        }
        else if (op == "+" || op == "-") {
            return 2;
        }
        else {
            // Неизвестный оператор, можно вернуть 1 или другое значение по умолчанию
            return 1;
        }
    }

    bool isValidInput(const QString& input)
    {
        QRegularExpression validRegex(ALLCURRENTSIMBOL);
        QRegularExpressionMatch match = validRegex.match(input);
        return match.hasMatch();
    }

    bool isOperator(const QString& input)
    {
        QRegularExpression validRegex(OPERANDS);
        QRegularExpressionMatch match = validRegex.match(input);
        return match.hasMatch();
    }

    bool isNegativeNumber(const QString& input)
    {
        QRegularExpression validRegex("-\\d+");
        QRegularExpressionMatch match = validRegex.match(input);
        bool isMinusBeforeNumber = match.hasMatch();
        return isMinusBeforeNumber;

    }

    bool isOperatorWithOneOperand(const QString& input)
    {
        QRegularExpression validRegex(OPERANDSRWITHONEARG);
        QRegularExpressionMatch match = validRegex.match(input);
        return match.hasMatch();
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

    // расчитываете выражение
    double performOperation(double operand1, const QString& operation, double operand2 = 0) {
        if (operation == "+") {
            return operand1 + operand2;
        }
        if (operation == "-") {
            return operand2 - operand1;
        }
        if (operation == "*") {
            return operand1 * operand2;
        }
        if (operation == "/") {
            if (operand1 == 0) {
                QMessageBox::warning(nullptr, "Warning", "You divide by zero");
            }
            return operand2 / operand1;
        }
        if (operation == "^") {
            return pow(operand2, operand1);
        }
        if (operation == "sqrt") {
            return sqrt(operand1);
        }
        if (operation == "log") {
            return log10(operand1);
        }
        if (operation == "ln" || operation == "lg") {
            return log(operand1);
        }
        if (operation == "sin") {
            return sin(operand1);
        }
        if (operation == "cos") {
            return cos(operand1);
        }
        if (operation == "tan" || operation == "tg") {
            return tan(operand1);
        }
        if (operation == "cot" || operation == "ctg") {
            return 1 / tan(operand1);
        }

        // Неподдерживаемая операция
        return std::numeric_limits<double>::quiet_NaN();
    }

    // use javascript, bu dont calculate function (for example ln10, cos(1), etc.)
    double calculateExpressionQJSEngine(const QString& expression) {
        QJSEngine engine;
        QJSValue result = engine.evaluate(expression);
        if (result.isError()) {
            qDebug() << "Error evaluating expression:" << result.toString();
            return 0.0;
        }
        return result.toNumber();
    }

    // this method doesn't work
    double calculateExpressionWithStack(const QString& expression) {
        std::stack<double> operands;
        std::stack<QChar> operators;

        int pos = 0;
        while (pos < expression.size()) {
            if (expression[pos].isSpace()) {
                pos++;
                continue;
            }

            if (expression[pos].isDigit()) {
                bool ok = false;
                double number = expression.mid(pos).toDouble(&ok);
                if (ok) {
                    operands.push(number);
                    while (pos < expression.size() && (expression[pos].isDigit() || expression[pos] == '.')) {
                        pos++; // Move the position to the end of the number
                    }
                    continue;
                }
                else {
                    // Handle error case here if needed
                    break;
                }
            }

            if (isOperator(expression[pos])) {
                while (!operators.empty() && getOperatorPrecedence(operators.top()) >= getOperatorPrecedence(expression[pos])) {
                    double rightOperand = operands.top();
                    operands.pop();
                    double leftOperand = operands.top();
                    operands.pop();
                    QChar op = operators.top();
                    operators.pop();
                    operands.push(performOperation(leftOperand, op, rightOperand));
                }
                operators.push(expression[pos]);
            }
            else if (expression[pos] == '(') {
                operators.push(expression[pos]);
            }
            else if (expression[pos] == ')') {
                while (!operators.empty() && operators.top() != '(') {
                    double rightOperand = operands.top();
                    operands.pop();
                    double leftOperand = operands.top();
                    operands.pop();
                    QChar op = operators.top();
                    operators.pop();
                    operands.push(performOperation(leftOperand, op, rightOperand));
                }
                if (!operators.empty()) {
                    operators.pop(); // Remove the corresponding '('
                }
            }
            else {
                // Invalid character in the expression
                // Handle error case here if needed
                break;
            }

            pos++;
        }

        while (!operators.empty()) {
            double rightOperand = operands.top();
            operands.pop();
            double leftOperand = operands.top();
            operands.pop();
            QChar op = operators.top();
            operators.pop();
            operands.push(performOperation(leftOperand, op, rightOperand)); 
        }

        if (!operands.empty()) {
            return operands.top();
        }
        else {
            // Handle error case here if needed (e.g., invalid expression)
            return 0;
        }
    }

    // перевод обычной строки в обратную польскую нотацию
    QStringList convertToRPN(const QString& expression) {

        QStringList outputQueue;  // Очередь для выходной обратной польской записи
        QStack<QString> operatorStack;  // Стек для операторов

        QStringList tokens = expression.split(' ', Qt::SkipEmptyParts);

        for (const QString& token : tokens) {
            if ( (!isOperator(token) || isNegativeNumber(token)) && token != "(" && token != ")") {
                // Проверяем, является ли токен числом
                bool isNumber;
                token.toDouble(&isNumber);
                if (isNumber) {
                    // Текущий токен - число, добавляем его в выходную очередь
                    outputQueue.append(token);
                }
                else {
                    // Обработка ошибки - токен не является числом
                    QMessageBox::warning(nullptr, "Warning", "Unknown operand");
                }
            }
            else if (token == "(") {
                // Текущий токен - открывающая скобка, добавляем ее в стек операторов
                operatorStack.push(token);
            }
            else if (token == ")") {
                // Текущий токен - закрывающая скобка

                // Перемещаем операторы из стека в выходную очередь до тех пор,
                // пока не встретим соответствующую открывающую скобку или стек не опустеет
                while (!operatorStack.isEmpty() && operatorStack.top() != "(") {
                    outputQueue.append(operatorStack.pop());
                }

                // Если стек опустел и не было найдено открывающей скобки, значит в выражении скобки не сбалансированы
                if (operatorStack.isEmpty()) {
                    QMessageBox::warning(nullptr, "Warning", "Unbalanced parentheses");
                    return QStringList();  // Возвращаем пустой список, чтобы обозначить ошибку
                }

                // Удаляем открывающую скобку из стека
                operatorStack.pop();
            }
            else {
                // Текущий токен - оператор

                while (!operatorStack.isEmpty() && operatorStack.top() != "(" &&
                    getOperatorPrecedence(token) <= getOperatorPrecedence(operatorStack.top())) {
                    // Текущий оператор имеет меньший или равный приоритет, чем оператор на вершине стека
                    // Перемещаем оператор с вершины стека в выходную очередь
                    outputQueue.append(operatorStack.pop());
                }

                // Добавляем текущий оператор в стек
                operatorStack.push(token);
            }
        }

        // Перемещаем оставшиеся операторы из стека в выходную очередь
        while (!operatorStack.isEmpty()) {
            outputQueue.append(operatorStack.pop());
        }

        return outputQueue;
    }

    double calculateExpressionWithRPN(const QString& expression)
    {
        // Создание списка токенов (числа и операторы)
        QStringList tokens = convertToRPN(expression);

        if (tokens.isEmpty())
        {
            return 0.0;
        }

        // Заполняем оба стека
        QStack<double> numbersStack;

        for (const QString& token : tokens) {
            if (!isOperator(token) || isNegativeNumber(token)) {
                // Текущий токен - число
                numbersStack.push(token.toDouble());
            }
            else {
                // Текущий токен - оператор

                if (isOperatorWithOneOperand(token)) {
                    double operand1 = numbersStack.pop();
                    double result = performOperation(operand1, token);
                    numbersStack.push(result);
                    continue;
                }

                double operand2 = numbersStack.pop();
                double operand1 = 0;
                if(!(numbersStack.isEmpty())) {
                    operand1 = numbersStack.pop();
                }

                double result = performOperation(operand2, token, operand1);
                numbersStack.push(result);

            }
        }

        return numbersStack.pop();
    }

    // функция которая корректирует полученную строку для корректных вычеслений 
    // (между числами и операндами должны быть пробелы, автоматически ставит скобки в нужных места и тп.)
    void updateText(QString& formattedText)
    {
        autoBalanceParentheses(formattedText);

        // Добавляем пробелы между числами и операндами, если они отсутствуют
        QRegularExpression spaceRegex("(\\d)([+\\-*/^sqrtloglnlgsincostancottgctg])");
        formattedText.replace(spaceRegex, "\\1 \\2 ");

        // Добавляем пробелы перед и после скобок
        formattedText.replace(QRegularExpression("\\("), " ( ");
        formattedText.replace(QRegularExpression("\\)"), " ) ");

        // Добавляем знак умножения, если между числом и скобкой есть пробел
        QRegularExpression numberBracketRegex("(\\d) *\\(");
        formattedText.replace(numberBracketRegex, "\\1 * (");
        QRegularExpression bracketNumberRegex("\\) (\\d)");
        formattedText.replace(bracketNumberRegex, ") * \\1");

        // Добавляем знак умножения, если между скобками нет знака операции
        formattedText.replace(QRegularExpression("\\)\\s*\\("), ") * (");

        // Удаляем лишние пробелы между числами
        QRegularExpression numberRegex("(\\d+)\\s*(\\d+)");
        formattedText.replace(numberRegex, "\\1\\2");

        // Удаления пробела между операндами и числами
        formattedText.replace(QRegularExpression("([+\\-*/sqrtloglnsincostanlgcottgctg])(\\d+)"), "\\1 \\2");
        formattedText.replace(QRegularExpression("(\\d+)\\s+([+\\-*/sqrtloglnsinlgcostancottgctg])"), "\\1 \\2");
        formattedText.replace(QRegularExpression("([+\\-*/]sqrtloglnsincostancottgctglg)\\s*(\\d+)"), "\\1 \\2");

        formattedText.replace(QRegularExpression("([cossintgtancthcotloglnsqrtlg^])\\s+(\\d+)"), "\\1 ( \\2 )");

        // Замена числа на формат 0.xxx, если число начинается с нуля (021 = 0.21)
        formattedText.replace(QRegularExpression("(0)([1-9])"), "\\1.\\2");

        // Замена ',' при вводе чисел на '.' для правильных вычислений
        formattedText.replace(QRegularExpression(","), ".");

        formattedText.replace(QRegularExpression("([+\\-*/sqrtloglnsincostanlgcottgctg(]) (-) (\\d+)"), "\\1 \\2\\3");

        // удаляем все лишние пробелы
        formattedText.replace(QRegularExpression("\\s+"), " ");
    }

    // Функция для подсчета значения выражения
    double evaluateFunction(const QString& expression, const QString& variable, double x)
    {
        QString xString = QString::number(x);
        QString evaluatedExpression = expression;
        evaluatedExpression.replace(variable, xString);

        return calculateExpressionWithRPN(evaluatedExpression);
    }

    double calculateSimpsonIntegral(QString&  function, const QString& variable, const double& upperLimit, const double& lowerLimit, const qint64& n = 1000)
    {

        double h = (upperLimit - lowerLimit) / n;
        double sum = evaluateFunction(function, variable, lowerLimit) + evaluateFunction(function, variable, upperLimit);

        for (size_t i = 1; i < n; i += 2) {
            double x = lowerLimit + i * h;
            sum += 4 * evaluateFunction(function, variable, x);
        }

        for (size_t i = 2; i < n - 1; i += 2) {
            double x = lowerLimit + i * h;
            sum += 2 * evaluateFunction(function, variable, x);
        }

        double result = sum * h / 3.0;
        return result;

    }
} // namespace CalculatorUtils
