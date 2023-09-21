#include <QRegularExpression>
#include <QMessageBox>
#include <QStack>
#include <stack>

//������������ ����
namespace CalculatorUtils {

    // ������ ��� ���������, ������� ����� �������������� � ���������.
    // �������� ������� [+\\-*/.^lgsqrtloglnsincostancottgctg].
    // ���� ����� � ������ isOperator �� ��������� ������������� ����� �� �������
    // ������� �������� ������ � �������������� ����� �������� ��� ��������� ������� isNegativeNumber
    const QString OPERATORSWITHFUNCTIONS = "[+\\-*/^lgsqrtloglnsincostancottgctg]";

    const QString OPERATORS = "[+\\-*/^]";

    // ������ ��� �������� ������������ ���� �������� � ������� ���������.
    // ����� ��������� ������ �����, ������, �������������� ��������� � �������.
    const QString ALLCURRENTSIMBOL = "^[0-9()+\\-*/.^lgsqrtloglnsincostancottgctg ]+$";

    // ������ ��� ���������, ��������� ������ ���� ��������.
    // �������� ������� [lgsqrtloglnsincostancottgctg].
    const QString FUNCTIONS = "[lgsqrtloglnsincostancottgctg]";

    // ����������� �������� ���������� �������� - ��� ���� ���������, ��� ������ ��� �����������
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
            // ����������� ��������, ����� ������� 1 ��� ������ �������� �� ���������
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
        QRegularExpression operatorsRegex(OPERATORS);
        QRegularExpression functionsRegex(FUNCTIONS);

        if (operatorsRegex.match(input).hasMatch() && operatorsRegex.match(input).captured() == input) {
            // ���� input ������������� ��������������� ��������� � �������� ������ ����������, ������� true.
            return true;
        }

        return functionsRegex.match(input).hasMatch();

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
        QRegularExpression validRegex(FUNCTIONS);
        QRegularExpressionMatch match = validRegex.match(input);
        return match.hasMatch();
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

    // ������������ ���������
    double performOperation(double operand1, const QString& operation, double operand2 = 0) {
        if (operation == "+") {
            return operand1 + operand2;
        }
        if (operation == "-") {
            return operand1 - operand2;
        }
        if (operation == "*") {
            return operand1 * operand2;
        }
        if (operation == "/") {
            if (operand1 == 0) {
                QMessageBox::warning(nullptr, "Warning", "You divide by zero");
            }
            return operand1 / operand2;
        }
        if (operation == "^") {
            return pow(operand1, operand2);
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

        // ���������������� ��������
        return std::numeric_limits<double>::quiet_NaN();
    }

    // ������� ������� ������ � �������� �������� �������
    QStringList convertToRPN(const QString& expression) {

        QStringList outputQueue;  // ������� ��� �������� �������� �������� ������
        QStack<QString> operatorStack;  // ���� ��� ����������

        QStringList tokens = expression.split(' ', Qt::SkipEmptyParts);

        for (const QString& token : tokens) {
            if ( (!isOperator(token) || isNegativeNumber(token)) && token != "(" && token != ")") {
                // ���������, �������� �� ����� ������
                bool isNumber;
                token.toDouble(&isNumber);
                if (isNumber) {
                    // ������� ����� - �����, ��������� ��� � �������� �������
                    outputQueue.append(token);
                }
                else {
                    // ��������� ������ - ����� �� �������� ������
                    QMessageBox::warning(nullptr, "Warning", "Unknown operand");
                }
            }
            else if (token == "(") {
                // ������� ����� - ����������� ������, ��������� �� � ���� ����������
                operatorStack.push(token);
            }
            else if (token == ")") {
                // ������� ����� - ����������� ������

                // ���������� ��������� �� ����� � �������� ������� �� ��� ���,
                // ���� �� �������� ��������������� ����������� ������ ��� ���� �� ��������
                while (!operatorStack.isEmpty() && operatorStack.top() != "(") {
                    outputQueue.append(operatorStack.pop());
                }

                // ���� ���� ������� � �� ���� ������� ����������� ������, ������ � ��������� ������ �� ��������������
                if (operatorStack.isEmpty()) {
                    QMessageBox::warning(nullptr, "Warning", "Unbalanced parentheses");
                    return QStringList();  // ���������� ������ ������, ����� ���������� ������
                }

                // ������� ����������� ������ �� �����
                operatorStack.pop();
            }
            else {
                // ������� ����� - ��������

                while (!operatorStack.isEmpty() && operatorStack.top() != "(" &&
                    getOperatorPrecedence(token) <= getOperatorPrecedence(operatorStack.top())) {
                    // ������� �������� ����� ������� ��� ������ ���������, ��� �������� �� ������� �����
                    // ���������� �������� � ������� ����� � �������� �������
                    outputQueue.append(operatorStack.pop());
                }

                // ��������� ������� �������� � ����
                operatorStack.push(token);
            }
        }

        // ���������� ���������� ��������� �� ����� � �������� �������
        while (!operatorStack.isEmpty()) {
            outputQueue.append(operatorStack.pop());
        }

        return outputQueue;
    }

    double calculateExpressionWithRPN(const QString& expression)
    {
        // �������� ������ ������� (����� � ���������)
        QStringList tokens = convertToRPN(expression);

        if (tokens.isEmpty())
        {
            return 0.0;
        }

        // ��������� ��� �����
        QStack<double> numbersStack;

        for (const QString& token : tokens) {


            if (!isOperator(token) || isNegativeNumber(token)) {
                // ������� ����� - �����
                numbersStack.push(token.toDouble());
            }
            else {
                // ������� ����� - ��������

                // ���� ����� ��� � �� ����� � �� �������� ������������ ������
                if (!isOperator(token)) {
                    // ������� ��������� �� ������ ������������
                    qDebug() << "������: ������������ ���� - " << token;
                    // ������� "nan" ��� ������ ����������� ��������, ����� ���������� ������
                    return std::numeric_limits<double>::quiet_NaN();
                }

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

                double result = performOperation(operand1, token, operand2);
                numbersStack.push(result);

            }
        }

        return numbersStack.pop();
    }

    // ������� ������� ������������ ���������� ������ ��� ���������� ���������� 
    // (����� ������� � ���������� ������ ���� �������, ������������� ������ ������ � ������ ����� � ��.)
    void updateText(QString& formattedText)
    {
        autoBalanceParentheses(formattedText);

        // ��������� ������� ����� ������� � ����������, ���� ��� �����������
        QRegularExpression spaceRegex("(\\d)([+\\-*/^sqrtloglnlgsincostancottgctg])");
        formattedText.replace(spaceRegex, "\\1 \\2 ");

        // ��������� ������� ����� � ����� ������
        formattedText.replace(QRegularExpression("\\("), " ( ");
        formattedText.replace(QRegularExpression("\\)"), " ) ");

        // ��������� ���� ���������, ���� ����� ������ � ������� ���� ������
        QRegularExpression numberBracketRegex("(\\d) *\\(");
        formattedText.replace(numberBracketRegex, "\\1 * (");
        QRegularExpression bracketNumberRegex("\\) (\\d)");
        formattedText.replace(bracketNumberRegex, ") * \\1");

        // ��������� ���� ���������, ���� ����� �������� ��� ����� ��������
        formattedText.replace(QRegularExpression("\\)\\s*\\("), ") * (");

        // ������� ������ ������� ����� �������
        QRegularExpression numberRegex("(\\d+)\\s*(\\d+)");
        formattedText.replace(numberRegex, "\\1\\2");

        // �������� ������� ����� ���������� � �������
        formattedText.replace(QRegularExpression("([+\\-*/sqrtloglnsincostanlgcottgctg])(\\d+)"), "\\1 \\2");
        formattedText.replace(QRegularExpression("(\\d+)\\s+([+\\-*/sqrtloglnsinlgcostancottgctg])"), "\\1 \\2");
        formattedText.replace(QRegularExpression("([+\\-*/]sqrtloglnsincostancottgctglg)\\s*(\\d+)"), "\\1 \\2");

        formattedText.replace(QRegularExpression("([cossintgtancthcotloglnsqrtlg^])\\s+(\\d+)"), "\\1 ( \\2 )");

        // ������� ��� ������ ���� ����� ������ ( 00000.21 = 0.21)
        formattedText.replace(QRegularExpression("^(0)(0*)(\\..*)?$"), "\\1\\3");

        // ������ ����� �� ������ 0.xxx, ���� ����� ���������� � ���� (021 = 0.21)
        formattedText.replace(QRegularExpression("(0)([1-9])(0*)"), "\\1.\\2\\3");

        // ������ ������ ����� 
        formattedText.replace(QRegularExpression("(\\.)([1-9]+)(0+)(\\.)"), "\\1\\2\\3");

        // �������� ������������������ �����, �� ������� ������� ����� � ���� �� ���� ��������� �����,
        // �� ��������� ����, ����� � ��������� �����.
        formattedText.replace(QRegularExpression("(0+)(.)([1-9])"), "0\\2\\3");

        // ������ ',' ��� ����� ����� �� '.' ��� ���������� ����������
        formattedText.replace(QRegularExpression(","), ".");

        formattedText.replace(QRegularExpression("([+\\-*/sqrtloglnsincostanlgcottgctg(]) (-) (\\d+)"), "\\1 \\2\\3");

        // ������� ��� ������ �������
        formattedText.replace(QRegularExpression("\\s+"), " ");
    }

    // ������� ��� �������� �������� ���������
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
