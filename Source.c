#define _CRT_SECURE_NO_DEPRECATE

#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define SIZEMAX 256
#define SIZESTACK 3
#define EXP "2,7182818"
#define PI  "3,1415926"

struct variable
{
	char* name;
	double value;
};

typedef struct variable variable;

variable** vars = NULL;
int varCount = 0;

int createVariable(char* varName);

void clearVariables();

void listVariables();

double stack[SIZESTACK] = { 0.0 };

void stackClear();

int stackSize();

void stackPush(double value);

double stackPop();

double operate(char operator);

int isOperator(char* str);

int isNumber(char* str);

int validVarName(char* str);

int printFile(char* fileName);

char* calc(char* str);

int main()
{
	char str[SIZEMAX] = { '\0' };
	setlocale(LC_ALL, "RUS");
	printf("������� ��������� � �������� �������� ������.\n");
	printf("��� ��������� �������������� ���������� � ��������, ������� ����� HELP.\n");
	printf("����� ��������� ������, ������� ����� ESCAPE.\n\n");

	while (1)
	{
		printf("����: ");
		//scanf("%s", str);
		gets_s(str, SIZEMAX);
		if (strcmp(str, "ESCAPE") == 0)
		{
			system("pause");
			return 0;
		}
		if (strcmp(str, "CLRVAR") == 0)
		{
			clearVariables();
			printf("������ ��������� ���������� ������.\n\n");
			continue;
		}
		if (strcmp(str, "LISTVAR") == 0)
		{
			listVariables();
			printf("\n");
			continue;
		}
		if (strcmp(str, "HELP") == 0)
		{
			printf("\n");
			printFile("help.txt");
			printf("\n");
			continue;
		}
		else
		{
			printf("�����: %s\n\n", calc(str));
		}
	}
}

char* calc(char* str) // ������� �������� � ���������� ���������
{
	int i = 0;
	int foundVariable = 0;
	char* newVarName = NULL;
	int codeErr = 0;
	double res = 0;
	char* tmp = NULL;
	char* mallocCheck = NULL;

	stackClear();

	tmp = strtok(str, "\t ");

	while (tmp != NULL)
	{
		mallocCheck = NULL;

		// �������� ������� �� ��� ���������
		if (strcmp(tmp, "EXP") == 0) tmp = EXP;
		if (strcmp(tmp, "PI") == 0) tmp = PI;

		// �������� ������� �� ��� ��������� ����������
		foundVariable = 0;
		for (i = 0; i < varCount; i++)
		{
			if (strcmp(tmp, vars[i]->name) == 0)
			{

				mallocCheck = malloc(sizeof(double) + 1);
				if (mallocCheck == NULL)
				{
					break;
				}
				tmp = mallocCheck;
				memset(tmp, '\0', sizeof(double) + 1);
				sprintf(tmp, "%lf", vars[i]->value);
				foundVariable = 1;
				break;
			}
		}

		if (isNumber(tmp) == 1)
		{
			if (stackSize() < 2)
			{
				stackPush(atof(tmp));
			}
			else
			{
				codeErr = 1;
				break;
			}
		}
		else if (isOperator(tmp) == 1)
		{
			if (strcmp(tmp, "=") != 0 && stackSize() == 2)
			{
				stackPush(operate(*tmp));
			}
			else if (strcmp(tmp, "=") == 0 && stackSize() == 1)
			{
				if (newVarName != NULL)
				{
					if (createVariable(newVarName) == 1)
					{
						vars[varCount - 1]->value = stackPop();
						free(newVarName);
						if (foundVariable == 1)
						{
							free(tmp);
						}
						memset(str, '\0', SIZEMAX);
						sprintf(str, "����� ���������� %s �� ��������� %lf ������� �������.", vars[varCount - 1]->name, vars[varCount - 1]->value);
						return str;
					}
					else
					{
						codeErr = 5;
					}
				}
				else
				{
					codeErr = 4;
					break;
				}
			}
			else
			{
				codeErr = 2;
				break;
			}
		}
		else if (newVarName == NULL && validVarName(tmp) == 1)
		{
			mallocCheck = malloc((strlen(tmp) + 1) * sizeof(char));
			if (mallocCheck == NULL)
			{
				codeErr = 5;
				break;
			}
			newVarName = mallocCheck;
			memset(newVarName, '\0', strlen(tmp) + 1);
			memcpy(newVarName, tmp, strlen(tmp));
		}
		else
		{
			codeErr = 3;
			break;
		}

		if (foundVariable == 1)
		{
			free(tmp);
		}

		tmp = strtok(NULL, "\t ");
	}

	if (codeErr != 0)
	{
		if (newVarName != NULL)
		{
			free(newVarName);
		}
		switch (codeErr)
		{
		case 1:
			return "������! � ����� ������ ���� �����.";
		case 2:
			return "������! ������������ ������������� ���������.";
		case 3:
			return "������! ������������ ���������.";
		case 4:
			return "������! ������� ��������� �������� �������������� ����������.";
		case 5:
			return "������! ��������� ������� �������� ����� ���������� (������������ ������).";
		}
	}

	if (newVarName != NULL)
	{
		free(newVarName);
	}

	res = stackPop();
	memset(str, '\0', SIZEMAX);
	sprintf(str, "%lf", res);
	return str;
}

int createVariable(char* varName) // ������� ���������� ���������� � ������ ���������
{
	variable* newVar = NULL;

	vars = realloc(vars, (varCount + 1) * sizeof(variable*));
	if (vars == NULL)
	{
		return 0;
	}

	newVar = malloc(sizeof(variable));
	if (newVar == NULL)
	{
		return 0;
	}

	newVar->name = malloc((strlen(varName) + 1) * sizeof(char));
	if (newVar->name == NULL)
	{
		free(newVar);
		return 0;
	}

	memset(newVar->name, '\0', (strlen(varName) + 1));
	memcpy(newVar->name, varName, strlen(varName));
	newVar->value = 0.0;

	vars[varCount] = newVar;

	varCount++;
	return 1;
}

void clearVariables() // ������� ������� ������ ��������� ����������
{
	int i = 0;

	for (i = 0; i < varCount; i++)
	{
		free(vars[i]->name);
		free(vars[i]);
	}

	varCount = 0;

	return;
}

void listVariables() // ������� ������ ������ ��������� ����������
{
	int i = 0;
	printf("------------------------------------------\n");
	printf("%-20s%-20s\n\n", "��� ����������", "��������");

	for (i = 0; i < varCount; i++)
	{
		printf("%-20s%-20lf\n", vars[i]->name, vars[i]->value);
	}

	printf("------------------------------------------\n");

	return;
}

void stackClear() // ������� ������� �����
{
	int i = 0;
	for (i = 0; i < SIZESTACK; i++)
	{
		stack[i] = 0.0;
	}
}

int stackSize() // ������� ������ ������� �����
{
	return trunc(stack[0]);
}

void stackPush(double value) // ������� ���������� ����� � ����
{
	stack[stackSize() + 1] = value;
	stack[0] += 1;
}

double stackPop() // ������� ������ ����� �� �����
{
	double res = 0;
	res = stack[stackSize()];
	stack[stackSize()] = 0;
	stack[0] -= 1;
	return res;
}

double operate(char operator) // ������� ��� ���������� �������������� �������� ��� ������� � �����
{
	double op2 = stackPop();
	double op1 = stackPop();
	switch (operator)
	{
	case '+':
		return op1 + op2;
	case '-':
		return op1 - op2;
	case '*':
		return op1 * op2;
	case '/':
		return op1 / op2;
	case '^':
		return pow(op1, op2);
	default:
		return;
	}
}

int isOperator(char* str) // ������� ��������� �������������� ������� � �������������� ����������
{
	// ���� ������ ���� � ������ "+-/*^=" � ����� ������ �� ����� ������� - 1
	if (strpbrk(str, "+-/*^=") != NULL && strlen(str) == 1)
	{
		// ����� �� ��������� "������"
		return 1;
	}
	else
	{
		// ����� �� ��������� "����"
		return 0;
	}
}

int isNumber(char* str) // �������, ������� ����������, �������� �� ������ ������
{
	int i = 0;
	int foundPoint = 0; // ���� ���������� �������-����������� � ������������ �����

	// ������������ ������ �� ������
	for (i = 0; i < strlen(str); i++)
	{
		// ������ - �� �����
		if (isdigit(str[i]) == 0)
		{
			// ���� ������ ������ "-" � �� �� ������������ � ������
			if (str[i] == '-' && i == 0 && strlen(str) != 1)
			{
				continue;
			}
			// ���� �������� ������-�����������
			else if (str[i] == ',')
			{
				// ������-����������� �� ��� �������� �����
				if (foundPoint == 0)
				{
					// ������-����������� ������
					foundPoint = 1;
					continue;
				}
				else
				{
					// ����� �� ��������� "����"
					return 0;
				}
			}
			else
			{
				// ����� �� ��������� "����"
				return 0;
			}
		}
	}
	// ����� �� ��������� "������"
	return 1;
}

int validVarName(char* str) // �������, ������������ ������������ ����� ���������� (�� ��������� ��)
{
	int i = 0;

	// ���� ������ ������ ������ - �� �����
	if (isalpha(str[0]) == 0)
	{
		// ����� �� ������� �� ��������� "����"
		return 0;
	}
	// ������ �� ������
	for (i = 1; i < strlen(str); i++)
	{
		// ���� ������ - �� ����� ��� �����
		if (isalnum(str[i]) == 0)
		{
			// ����� �� ������� �� ��������� "����"
			return 0;
		}
	}
	// ����� �� ������� �� ��������� "������"
	return 1;
}

int printFile(char* fileName) // ������� ������ ����������� ����� � �������
{
	FILE* input;					// ��������� �� ���� �����

	char tmpS[SIZEMAX] = { 0 };		// ����������� ������ ������� 256 ��������� ��� ����� ����� �� �����

	// �������� ����� ��� ������
	input = fopen(fileName, "r");

	// ���� ���� ����� �� ������.
	if (input == NULL)
	{
		return -1;
	}//if

	while (fgets(tmpS, SIZEMAX, input) != NULL)		// ���� �� ����� �����
	{
		printf("%s", tmpS);
	}//while

	// �������� ����� ��� ������
	fclose(input);

	// ������� ���������� 0 � ������ ��������� ����������
	return 0;
}//printFile
