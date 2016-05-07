//============================================================================
// Name        : CompilerProject.cpp
// Author      : Begun Unal
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstring>
#include <vector>
#include <stack>
#include <sstream>
#include <regex>
using namespace std;

enum type {scalar, _vector, matrix};

typedef struct variable {
	int dim1, dim2;
	string name;
	type t;
	vector <string> values;
	variable(){

	}
	variable(int _dim1, int _dim2, type _t){//this is initial statement of a variable
			t = _t;
			dim1 =_dim1;
			dim2 = _dim2;
		}
} variable;

vector< vector<string> > lineTokens ;//collecting  vector of tokenized lines
string cCode = "";
vector<variable> variables;//tokenized string for a line
map <string,variable> mp;// maps a variable's name to its variable that it belongs to
typedef struct token{
	string data;
	bool notNull = true;
	int row;
	int column;
	type _type;
}token;

int counter = 0;
void myPop();
void tokenization(string line);
bool isDeclaration(int lineNum);
void isExpression(int lineNum, int &tokenNum);
void isTerm(int lineNum, int &tokenNum);
void isMoreTerm(int lineNum, int &tokenNum);
void isFactor(int lineNum, int &tokenNum);
void isMoreFactor(int lineNum, int &tokenNum);
void printFunction(int lineNum);
void statementFor(int lineNum);
void printsepFunction(int lineNum);
void assignment(int lineNum);
bool isInitiallization(int lineNum);
bool isDeclared(variable id);
void decision(int lineNum, int tokneNum);

typedef struct error {
	string warning;
	bool wrong;
	int line;
	error(){

	}
	error(string _warning, int _line){
		warning = _warning;
		wrong = false;
		line = _line;
		}

}error;
error globalError;
stack<token> tokens;
int main(int argc,char** argv) {
	ifstream reader(argv[1]);
	string line;
	int count =0;
	while(reader.peek() != EOF){

		getline(reader,line);
		if(line[0] == '#'){
			count++;
			continue;
		}
		lineTokens.resize(lineTokens.size()+1);
		tokenization(line);
		counter++;
	}
		string headOfC = "#include <stdio.h>\n\
#include <stdlib.h>\n\
#include <math.h>\n\
\n\
typedef struct variable{\n\
	int dim1;\n\
	int dim2;\n\
	double** toMatrix;\n\
}variable;\n\
variable makeMatrix(int dim1, int dim2);\n\
void print(variable x);\n\
void printsep();\n\
variable choose(variable first, variable second, variable third, variable forth);\n\
double sqrt(double k);\n\
variable _sqrt(variable x);\n\
variable tr( variable var);\n\
variable substraction(variable first, variable second);\n\
variable addition( variable first, variable second);\n\
variable numToVar(double num);\n\
void atama(variable leftHandSide, variable rightHandSide);\n\
variable multiplication( variable first, variable second);\n\
double varToNum(variable x);\n\
variable makeMatrix(int dim1, int dim2){\n\
	variable x;\n\
	x.dim1 = dim1;\n\
	x.dim2 = dim2;\n\
	x.toMatrix = (double**)malloc(x.dim1*sizeof(double*));\n\
	int k;\n\
	for(k=0; k<x.dim1; k++){\n\
		x.toMatrix[k] = (double*)malloc(x.dim2*sizeof(double));\n\
	}\n\
\n\
	return x;\n\
}\n\
void print(variable x){\n\
	int i,j;\n\
	for(i=0; i<x.dim1; i++){\n\
		for(j=0; j<x.dim2; j++){\n\
			printf(\"%g \\n\", x.toMatrix[i][j]);\n\
		}\n\
	}\n\
}\n\
void printsep(){\n\
	printf(\"----------\\n\");\n\
}\n\
variable choose(variable first, variable second, variable third, variable forth){\n\
	if(first.toMatrix[0][0] == 0){\n\
		return second;\n\
	}else if(first.toMatrix[0][0] > 0){\n\
		return third;\n\
	}else if(first.toMatrix[0][0] < 0){\n\
		return forth;\n\
	}\n\
}\n\
double sqrt(double k){\n\
	double beg=0;\n\
	double end=k;\n\
	double mid;\n\
	double eps = 0.00000001;\n\
		if(end < 1 ){\n\
		end = 1;\n\
	}\n\
	while(beg+eps < end){\n\
		mid = (beg+end) / 2;\n\
		if(mid * mid < k) beg = mid;\n\
		else end = mid;\n\
	}\n\
	return beg;\n\
}\n\
variable _sqrt(variable x){\n\
	variable y;\n\
	y = makeMatrix(1,1);\n\
	y.toMatrix[0][0] = x.toMatrix[0][0];\n\
	y.toMatrix[0][0] = sqrt(y.toMatrix[0][0]);\n\
	return y;\n\
}\n\
\n\
\n\
double varToNum(variable x){\n\
	return x.toMatrix[0][0];\n\
}\n\
variable numToVar(double num){\n\
	variable result;\n\
	result = makeMatrix(1,1);\n\
	result.toMatrix[0][0] = num;\n\
	return result;\n\
}\n\
\n\
variable tr( variable var){\n\
	variable temp;\n\
	temp = makeMatrix(var.dim2,var.dim1);\n\
	int i,j;\n\
	for(i=0; i<var.dim2; i++){\n\
		for(j=0; j<var.dim1; j++){\n\
			temp.toMatrix[i][j] = var.toMatrix[j][i];\n\
		}\n\
	}\n\
	return temp;\n\
}\n\
\n\
variable substraction(variable first, variable second){\n\
	variable result;\n\
	result = makeMatrix(first.dim1,first.dim2);\n\
	int i,j;\n\
	for(i=0; i<first.dim1; i++){\n\
		for(j=0; j<first.dim2; j++){\n\
			result.toMatrix[i][j] = first.toMatrix[i][j]-second.toMatrix[i][j];\n\
		}\n\
	}\n\
	return result;\n\
}\n\
variable addition( variable first, variable second){\n\
	variable result;\n\
	result = makeMatrix(first.dim1,first.dim2);\n\
	int i,j;\n\
	for(i=0; i<first.dim1; i++){\n\
		for(j=0; j<first.dim2; j++){\n\
			result.toMatrix[i][j] = first.toMatrix[i][j]+second.toMatrix[i][j];\n\
		}\n\
	}\n\
	return result;\n\
}\n\
\n\
void atama(variable leftHandSide, variable rightHandSide){\n\
	int i,j;\n\
	for(i=0;i<rightHandSide.dim1; i++){\n\
		for(j=0; j<leftHandSide.dim2; j++){\n\
			leftHandSide.toMatrix[i][j] = rightHandSide.toMatrix[i][j];\n\
		}\n\
	}\n\
}\n\
variable multiplication( variable first, variable second){\n\
	if(first.dim1 == 1 && first.dim2 == 1){\n\
		double temp = first.toMatrix[0][0];\n\
		int j,i;\n\
		for(i=0; i<second.dim1; i++){\n\
			for(j=0; j<second.dim2; j++){\n\
				second.toMatrix[i][j] *= temp;\n\
			}\n\
		}\n\
		return second;\n\
	}else if(second.dim1 == 1 && second.dim2 == 1){\n\
		double temp = second.toMatrix[0][0];\n\
		int i,j;\n\
		for(i=0; i<first.dim1; i++){\n\
			for(j=0; j<first.dim2; j++){\n\
				first.toMatrix[i][j] *= temp;\n\
			}\n\
		}\n\
		return first;\n\
	}else{\n\
		variable result;\n\
		result = makeMatrix(first.dim1,second.dim2);\n\
		int i,j,k;\n\
		for(i=0; i<first.dim1; i++){\n\
			for(j=0; j<second.dim2; j++){\n\
				result.toMatrix[i][j] = 0;\n\
				for(k=0; k<first.dim2; k++){\n\
					result.toMatrix[i][j] += first.toMatrix[i][k]*second.toMatrix[k][j];\n\
				}\n\
			}\n\
		}\n\
		return result;\n\
	}\n\
}\n\
int main(void) {";
		fstream out;
		string infilename = argv[2];
		out.open(infilename,fstream::out);
		cCode += headOfC + "\n";
		//cout<<headOfC<<endl;
		unsigned int i;


		for( i=0; i<lineTokens.size(); i++){

			string str = lineTokens[i][0];
			if(str == "#"){
							//cout<<"//comment"<<endl;
			}else if(str == "scalar" || str == "vector" || str == "matrix"){
				cCode += "variable ";
				//cout<<"variable ";
				isDeclaration(i);
				cCode += "= makeMatrix(" + to_string(mp[lineTokens[i][1]].dim1) + "," + to_string(mp[lineTokens[i][1]].dim2) + ");\n";
				//cout<<"= makeMatrix("<< mp[lineTokens[i][1]].dim1 << ", "<< mp[lineTokens[i][1]].dim2<< ");"<<endl;
			}else if(str == "print"){
				printFunction(i);
			}else if(str == "for"){
				statementFor(i);
			}else if(str == "printsep"){
				printsepFunction(i);
			}else if(lineTokens[i][1] == "="){
				if(lineTokens[i].size()<3){
					assignment(i);
				}else if(lineTokens[i][2] == "{"){
					isInitiallization(i);
				}else{
					assignment(i);
				}

			}else if(str == "}"){
				cCode += "}\n";
				//cout<<"}"<<endl;
			}
		}
		cCode += "return 0;}";
		//cout<<	"return 0;}" <<endl;

if(globalError.wrong){
	cCode = "Line" + to_string(globalError.line+count) + ":" + globalError.warning;
}
		out<<cCode<<endl;


	return 0;
}
bool isDouble(string str){//checking str is a string of a double
	int count = 0;
	for(int i=0; i<str.length(); i++){
		if(str[i]!='.' && !((int)(str[i])-(int)('0')<10)){
			return false;
		}if(str[i] == '.'){
			count++;
			if(count>1){
				return false;
			}
		}
	}
	return true;
}
bool isInteger(string str){//checks if str is a string of an integer
	int i;
	for(i=0; i<str.length(); i++){
		if(!((int)(str[i])-(int)('0')<10)){
			return false;
		}
	}
	return true;

}

void tokenization(string line){//tokenizes each line
	for(int i=0; i<line.length(); i++){
		if(i+1<=line.length()-1){
			char tmp = line[i+1];
			if(tmp == '=' || tmp == '[' || tmp == ':' || tmp == ']' || tmp == ',' || tmp == '{' || tmp == '}' || tmp == '+' || tmp == '-' || tmp == '*' || tmp == '(' || tmp == ')' || tmp == '#'){
				if(!isspace(line[i])){
					line.insert(i+1," ");
					i++;
				}
				if(!isspace(line[i+2])){
					line.insert(i+2," ");
					i++;
				}
			}
		}
	}
	char *cstr = new char[line.length()];
	strcpy(cstr, line.c_str());
	char * pch;
	pch = strtok(cstr," ");
	  while (pch != NULL)
	  {
		  string str = pch;
	    lineTokens[counter].push_back(str);
	    pch = strtok (NULL, " ");
	  }


}

void printsepFunction(int lineNum){//this function is not necessary, it call printsep function
	int tokenNum = 1;
	string str = "";
	if(lineTokens[lineNum][tokenNum] != "("){

		return;
	}else{
		str = "printsep();";
		cCode += str + "\n";
		//cout<<str<<endl;
	}
}


token checkDimAddition(token tokLeft, token tokRight, string operat){//check addition restirictions and returns last version of the curretly created token
	if(tokLeft._type == scalar){
		if(tokRight._type == scalar){
			if(operat == "+"){
			tokLeft.data = "addition(" + tokLeft.data + "," + tokRight.data + ")";
			}
			else if(operat == "-"){
			tokLeft.data = "substraction(" + tokLeft.data + "," + tokRight.data + ")";
			}
		}else if(tokRight._type == _vector && tokRight.row == 1){
			if(operat == "+")
			tokLeft.data = "addition(" + tokLeft.data + "," + tokRight.data + ")";

			else if(operat == "-")
			tokLeft.data = "substraction(" + tokLeft.data + "," + tokRight.data + ")";

		}else if(tokRight._type == matrix && tokRight.column == 1 && tokRight.row == 1){
			if(operat == "+")
			tokLeft.data = "addition(" + tokLeft.data + "," + tokRight.data + ")";

			else if(operat == "-")
			tokLeft.data = "substraction(" + tokLeft.data + "," + tokRight.data + ")";

		}else{
			tokLeft.notNull = false;
		}
	}else if(tokLeft._type == _vector){
		if(tokRight._type ==_vector && tokRight.row == tokLeft.row){
			if(operat == "+")
			tokLeft.data = "addition(" + tokLeft.data + "," + tokRight.data + ")";

			else if(operat == "-")
			tokLeft.data = "substraction(" + tokLeft.data + "," + tokRight.data + ")";

		}else if(tokRight._type == matrix && tokRight.column == 1 && tokRight.row == tokLeft.row){
			if(operat == "+")
			tokLeft.data = "addition(" + tokLeft.data + "," + tokRight.data + ")";

			else if(operat == "-")
			tokLeft.data = "substraction(" + tokLeft.data + "," + tokRight.data + ")";

		}else if(tokRight._type == scalar && tokLeft.row == 1){
			if(operat == "+")
			tokLeft.data = "addition(" + tokLeft.data + "," + tokRight.data + ")";

			else if(operat == "-")
			tokLeft.data = "substraction(" + tokLeft.data + "," + tokRight.data + ")";

		}else {
			tokLeft.notNull = false;
		}
		return tokLeft;
	}else if(tokLeft._type == matrix){
		if(tokRight._type == matrix && tokRight.column == tokLeft.column && tokRight.row == tokLeft.row){
			if(operat == "+")
				if(operat == "+")
				tokLeft.data = "addition(" + tokLeft.data + "," + tokRight.data + ")";

				else if(operat == "-")
				tokLeft.data = "substraction(" + tokLeft.data + "," + tokRight.data + ")";

		}else if(tokRight._type == _vector && tokLeft.column == 1 && tokRight.row == tokLeft.row){
			if(operat == "+")
				if(operat == "+")
				tokLeft.data = "addition(" + tokLeft.data + "," + tokRight.data + ")";

				else if(operat == "-")
				tokLeft.data = "substraction(" + tokLeft.data + "," + tokRight.data + ")";

		}else if(tokRight._type == scalar && tokLeft.column == 1 && tokRight.row == 1){
			if(operat == "+")
			tokLeft.data = "addition(" + tokLeft.data + "," + tokRight.data + ")";

			else if(operat == "-")
			tokLeft.data = "substraction(" +  tokLeft.data + "," + tokRight.data + ")";

		}else{
			tokLeft.notNull = false;
		}
	}
	//cout<<tokLeft.data<<endl;
	return tokLeft;
}
void fixType(token &t){//detecting scalars and vector which were matrix before
	if(t.column == 1 && t.row == 1){
		t._type = scalar;
	}else if(t.column == 1 && t.row != 1){
		t._type = _vector;
	}else{
		t._type = matrix;
	}
}
token checkDimMultiplication(token tokLeft, token tokRight){//checks multiplication restrictions
	if(tokLeft._type == scalar){
		if(tokRight._type == scalar){
			tokLeft.data = "multiplication(" + tokLeft.data + "," + tokRight.data + ")";
		}else if(tokRight._type == _vector){
			tokLeft.data = "multiplication(" + tokLeft.data + "," + tokRight.data + ")";
			tokLeft._type = _vector;
			tokLeft.row = tokRight.row;
		}else if(tokRight._type == matrix){
			tokLeft.data = "multiplication(" + tokLeft.data + "," + tokRight.data + ")";
			tokLeft._type = matrix;
			tokLeft.column = tokRight.column;
			tokLeft.row = tokRight.row;
		}
	}else if(tokLeft._type == _vector){
		if(tokRight._type == scalar){
			tokLeft.data = "multiplication(" + tokLeft.data + "," + tokRight.data + ")";
		}else if((tokRight._type == _vector || tokRight._type == matrix) && tokLeft.column == tokRight.row){
			tokLeft.data = "multiplication(" + tokLeft.data + "," + tokRight.data + ")";
			tokLeft.column = tokRight.column ;
			if(tokLeft.column == 1 && tokLeft.row == 1){
				tokLeft._type = scalar;
			}
		}else{
			tokLeft.notNull = false;
		}
	}else if(tokLeft._type == matrix){
		if(tokRight._type == scalar){
			tokLeft.data = "multiplication(" + tokLeft.data + "," + tokRight.data + ")";
		}else if((tokRight._type == _vector || tokRight._type == matrix) && tokLeft.column == tokRight.row){
			tokLeft.data = "multiplication(" + tokLeft.data + "," + tokRight.data + ")";
			tokLeft.column = tokRight.column;
			if(tokLeft.column == 1 && tokLeft.row == 1){
				tokLeft._type = scalar;
			}
		}else{
			tokLeft.notNull = false;
		}
	}
	fixType(tokLeft);
	return tokLeft;
}
token checkChoose(token ex1,token ex2,token ex3,token ex4){//checks restrictions of choose function's call
	if(ex1._type == scalar && ex2._type == scalar && ex3._type == scalar && ex4._type == scalar){
		ex1.data = "choose(" + ex1.data + "," + ex2.data + "," + ex3.data + "," + ex4.data + ")";
	}else{
		ex1.notNull = false;
	}
	return ex1;
}
token checkTranspose(token param){//
	param.data = "tr(" + param.data + ")";
	int temp;
	temp = param.column;
	param.column = param.row;
	param.row = temp;
	return param;
}
token checkSquareRoot(token param){
	if(param._type == scalar){
		param.data = "numToVar(sqrt(varToNum(" + param.data + ")))";
	}else{
		param.notNull = false;
	}
	return param;
}
token checkDimVector(token _vector, token inIndex){
	if(inIndex._type == scalar){
		_vector.data = "numToVar(" + _vector.data + ".toMatrix[(int)varToNum(substraction(" + inIndex.data + ",numToVar(1)))][0])";
		_vector._type = scalar;
		_vector.column = 1;
		_vector.row = 1;
	}else{
		_vector.notNull = false;
	}
	return _vector;
}
token checkDimMatrix(token matrix,token first, token second){
	if(first._type == scalar && second._type == scalar){
		matrix.data = "numToVar(" + matrix.data + ".toMatrix[(int)varToNum(substraction(" + first.data + ",numToVar(1)))][(int)varToNum(substraction(" + second.data + ",numToVar(1)))])";
		matrix._type = scalar;
		matrix.column = 1;
		matrix.row = 1;
	}else{
		matrix.notNull = false;
	}
	return matrix;
}
void myPop(){//combining the tokens inside the stack/ tokens are collected in prefix form/ each binary and unary operations know their expected tokens
	token tmp;
	tmp = tokens.top();
	tokens.pop();
	if(tmp.data == "+"){
		myPop();
		token temp1;
		temp1 = tokens.top();
		tokens.pop();
		myPop();
		token temp2;
		temp2 = tokens.top();
		tokens.pop();
		tmp = checkDimAddition(temp2, temp1, "+");
		tokens.push(tmp);
	}else if(tmp.data == "-"){
		myPop();
		token temp1;
		temp1 = tokens.top();
		tokens.pop();
		myPop();
		token temp2;
		temp2 = tokens.top();
		tokens.pop();
		tmp = checkDimAddition(temp2, temp1, "-");
		if(tmp.notNull){
		tokens.push(tmp);
		}
	}else if(tmp.data == "*"){
		myPop();
		token temp1;
		temp1 = tokens.top();
		tokens.pop();
		myPop();
		token temp2;
		temp2 = tokens.top();
		tokens.pop();
		tmp = checkDimMultiplication(temp2, temp1);
		if(tmp.notNull){
		tokens.push(tmp);
		}
	}else if(tmp.data == "["){
		myPop();
		token temp1;
		temp1 = tokens.top();
		tokens.pop();
		myPop();
		token temp2;
		temp2 = tokens.top();
		tokens.pop();
		if(temp1._type == _vector){
			tmp = checkDimVector(temp1,temp2);
			if(tmp.notNull){
				tokens.push(tmp);
			}
		}else if(temp1._type == matrix){
			myPop();
			token temp3;
			temp3 = tokens.top();
			tokens.pop();
			tmp = checkDimMatrix(temp1,temp3,temp2);
			if(tmp.notNull){
				tokens.push(tmp);
			}
		}

	}else if(tmp.data == "sqrt"){
		myPop();
		token temp1;
		temp1 = tokens.top();
		tokens.pop();
		tmp = checkSquareRoot(temp1);
		if(tmp.notNull){
			tokens.push(tmp);
		}
	}else if(tmp.data == "tr"){

		myPop();
		token temp1;
		temp1 = tokens.top();
		tokens.pop();
		tmp = checkTranspose(temp1);
		if(tmp.notNull){
			tokens.push(tmp);
		}

	}else if(tmp.data == "choose"){
		myPop();
		token temp1;
		temp1 = tokens.top();
		tokens.pop();
		myPop();
		token temp2;
		temp2 = tokens.top();
		tokens.pop();
		myPop();
		token temp3;
		temp3 = tokens.top();
		tokens.pop();
		myPop();
		token temp4;
		temp4 = tokens.top();
		tokens.pop();
		tmp = checkChoose(temp4,temp3,temp2,temp1);
		if(tmp.notNull){
			tokens.push(tmp);
		}
	}else if(tmp.data == "("){
		myPop();
		token temp1;
		temp1 = tokens.top();
		tokens.pop();
		tmp.data = tmp.data + temp1.data + ")";
		tmp._type = temp1._type;
		tmp.column = temp1.column;
		tmp.row = temp1.row;
		tokens.push(tmp);
	}
	else{
		tokens.push(tmp);
	}
}
void isTerm(int lineNum, int &tokenNum){
	//cout<<"//isTerm"<<endl;
	isFactor(lineNum,tokenNum);
	isMoreFactor(lineNum,tokenNum);
}

void isMoreTerm(int lineNum, int &tokenNum){
	//cout<<"//isMoreTerm"<<endl;
	string temp;
	if(tokenNum < lineTokens[lineNum].size()){
		temp = lineTokens[lineNum][tokenNum];
	}else{
		return;
	}
	tokenNum++;
	if(temp == "+" || temp == "-"){
		token t;
		t.data = temp;
		isTerm(lineNum,tokenNum);
		tokens.push(t);
		isMoreTerm(lineNum,tokenNum);
	}else{
		tokenNum--;
	}
}
void isFactor(int lineNum, int &tokenNum){//the lowest level of the recursive parse tree
	//cout<<"//isFactor"<<endl;
	string temp;
	if(tokenNum < lineTokens[lineNum].size()){
		temp = lineTokens[lineNum][tokenNum];
	}else{
		return;
	}
	tokenNum++;
	if(temp == "("){
		token t;
		t.data = temp;
		 isExpression(lineNum, tokenNum);
		 tokens.push(t);
		 if(tokenNum < lineTokens[lineNum].size()){
		 if(lineTokens[lineNum][tokenNum] != ")"){
			 globalError = error(") is missing", lineNum);
			 globalError.wrong = true;
		 }
		 }else{
			 globalError = error("index out of bound", lineNum);
			 globalError.wrong = true;
			 return;
		 }
		 tokenNum++;
	}else if(mp.count(temp) == 1){
		if(mp[temp].t == scalar){
			token t;
			t.data = mp[temp].name;
			t._type = scalar;
			t.column = 1;
			t.row = 1;
			tokens.push(t);
		}else if(mp[temp].t == _vector){
			token t1;
			t1.data = temp;
			t1.column = mp[temp].dim2;
			t1.row = mp[temp].dim1;
			t1._type = _vector;
			//tokens.push(t1);
			string temp2;
			if(tokenNum < lineTokens[lineNum].size()){
				temp2 = lineTokens[lineNum][tokenNum];
			}else{
				tokens.push(t1);
				return;
			}
			tokenNum++;
			if(temp2 == "["){

				isExpression(lineNum,tokenNum);
				token t;
				t.data = temp2;
				tokens.push(t1);
				tokens.push(t);

				if(tokenNum < lineTokens[lineNum].size()){
					if(lineTokens[lineNum][tokenNum] != "]"){
						globalError = error("] is missing", lineNum);
						globalError.wrong = true;
					}
				}else{
					globalError = error("index out of bound", lineNum);
					globalError.wrong = true;
					return;
				}
				tokenNum++;
			}else{
				tokens.push(t1);
				tokenNum--;
			}
		}else if(mp[temp].t == matrix){
			token t1;
			t1.data = temp;
			t1.column = mp[temp].dim2;
			t1.row = mp[temp].dim1;
			t1._type = matrix;
			//tokens.push(t1);
			string temp2;
			if(tokenNum < lineTokens[lineNum].size()){
				temp2 = lineTokens[lineNum][tokenNum];
			}else{
				tokens.push(t1);
				return;
			}
			tokenNum++;
			if(temp2 == "["){

				isExpression(lineNum, tokenNum);
				if(tokenNum < lineTokens[lineNum].size()){
						if(lineTokens[lineNum][tokenNum] == ","){
							tokenNum++;
							isExpression(lineNum, tokenNum);
							token t;
							t.data = temp2;
							tokens.push(t1);
							tokens.push(t);

						}else{
							globalError = error("matrix error", lineNum);
							globalError.wrong = true;
						}
				}else{
					globalError = error("matrix out of bound", lineNum);
					globalError.wrong = true;
					return;
				}
				if(tokenNum < lineTokens[lineNum].size()){
					if(lineTokens[lineNum][tokenNum] != "]"){
						globalError = error("] is missing", lineNum);
						globalError.wrong = true;
					}
				}else{
					globalError = error("index out of bound", lineNum);
					globalError.wrong = true;
					return;
				}
				tokenNum++;

			}else{
				tokens.push(t1);
				tokenNum--;
			}

		}
	}else if(temp == "tr"){
		string temp2;
		if(tokenNum < lineTokens[lineNum].size()){
			temp2 =lineTokens[lineNum][tokenNum];
		}else{
			return;
		}
		tokenNum++;
		if(temp2 == "("){
			token t;
			t.data = temp;

			//tokens.push(braket);
			isExpression(lineNum, tokenNum);
			if(tokenNum < lineTokens[lineNum].size()){
			if(lineTokens[lineNum][tokenNum] != ")"){
				globalError = error(") is missing for tr()", lineNum);
				globalError.wrong = true;
			}
			}else{
				globalError = error("index out of bound", lineNum);
				globalError.wrong = true;
				return;
			}
			tokenNum++;
			tokens.push(t);
		}else{
			globalError = error("( is missing", lineNum);
			globalError.wrong = true;
		}
	}else if(temp == "sqrt"){
		string temp2;
		if(tokenNum < lineTokens[lineNum].size()){
			temp2 =lineTokens[lineNum][tokenNum];
		}else{
			return;
		}
		tokenNum++;
		if(temp2 == "("){

			token t;
			t.data = temp;

			//tokens.push(braket);
			isExpression(lineNum, tokenNum);
			if(tokenNum < lineTokens[lineNum].size()){
			if(lineTokens[lineNum][tokenNum] != ")"){
				globalError = error(") is missing", lineNum);
				globalError.wrong = true;
			}
			}else{
				globalError = error("index out of bound", lineNum);
				globalError.wrong = true;
				return;
			}
			tokenNum++;
			tokens.push(t);
		}else{
			globalError = error("( is missing for sqrt", lineNum);
			globalError.wrong = true;
		}
	}else if(temp == "choose"){
		string temp2;
		if(tokenNum < lineTokens[lineNum].size()){
			temp2 = lineTokens[lineNum][tokenNum];
		}else{
			return;
		}
		tokenNum++;
		if(temp2 == "("){
			token t;
			t.data = temp;
			for(int i=0; i<3; i++){
				isExpression(lineNum, tokenNum);
				if(tokenNum < lineTokens[lineNum].size()){
				if(lineTokens[lineNum][tokenNum] != "," && i<4){
					globalError = error("coma is missing", lineNum);
					globalError.wrong = true;
				}
				}else{
					return;
				}
				tokenNum++;

			}
			isExpression(lineNum,tokenNum);
			if(tokenNum < lineTokens[lineNum].size()){
			if(lineTokens[lineNum][tokenNum] != ")"){
				globalError = error(") is missing for choose", lineNum);
				globalError.wrong = true;
			}
			}else{
				return;
			}
			tokenNum++;
			tokens.push(t);

		}
	}else if(isDouble(temp)){
		token t;
		temp = "numToVar(" + temp + ")";
		t.data = temp;
		t._type = scalar;
		t.column = 1;
		t.row = 1;
		tokens.push(t);
	}else{
		globalError = error("invalid argument", lineNum);
		globalError.wrong = true;
	}
}
void isMoreFactor(int lineNum, int &tokenNum){
	//cout<<"//isMoreFactor"<<endl;
	string temp;
	if(tokenNum <lineTokens[lineNum].size()){
		temp = lineTokens[lineNum][tokenNum];
	}else{
		return;
	}
	tokenNum++;

	if(temp == "*"){
		token t;
		t.data = temp;
		isFactor(lineNum, tokenNum);
		tokens.push(t);
		isMoreFactor(lineNum,tokenNum);
	}else{
		tokenNum--;
	}
}

bool isDeclaration(int lineNum){//writing declarations of variables in a C document
	if(mp.find(lineTokens[lineNum][1]) != mp.end()){
		globalError = error("variable is already used", lineNum);
		globalError.wrong = true;
		return false;
	}else{

	if(lineTokens[lineNum][0] == "scalar"){

		if(lineTokens[lineNum].size() == 2){
			variable x(1,1, scalar);
			x.name = lineTokens[lineNum][1];
			mp[lineTokens[lineNum][1]] = x;
			cCode += x.name;
			//cout<<x.name<<endl;
			variables.push_back(x);
		}else{
			globalError = error("line is longer than expected", lineNum);
			globalError.wrong = true;
			return false;
		}
	}else if(lineTokens[lineNum][0] == "vector"){
		if(!isInteger(lineTokens[lineNum][3])){
			globalError = error("vector dim should be an int", lineNum);
			globalError.wrong = true;
			return false;
		}
		if(lineTokens[lineNum][2] == "[" && lineTokens[lineNum][4] == "]" && lineTokens[lineNum].size() == 5){
			stringstream convert(lineTokens[lineNum][3]);
			int tmp;
			if(!(convert >> tmp)){
				tmp = 0;
			}
			variable x(tmp,1,_vector);
			x.name = lineTokens[lineNum][1];
			mp[lineTokens[lineNum][1]] = x;
			variables.push_back(x);
			cCode += x.name;
			//cout<<x.name<<endl;
		}else{
			globalError = error("this is not an appropriate vector", lineNum);
			globalError.wrong = true;
			return false;
		}
	}else if(lineTokens[lineNum][0] == "matrix"){
		if(!(isInteger(lineTokens[lineNum][3])) || !(isInteger(lineTokens[lineNum][5]))){
			globalError = error("dimensions should be an integer", lineNum);
			globalError.wrong = true;
			return false;
		}else{
		if(lineTokens[lineNum][2] == "[" && lineTokens[lineNum][6] == "]" && lineTokens[lineNum][4] == "," && lineTokens[lineNum].size() == 7){
			stringstream convert(lineTokens[lineNum][3]);
			int tmp, tmp2;
			if(!(convert >> tmp)){
				tmp = 0;
			}
			stringstream convert2(lineTokens[lineNum][5]);
			if(!(convert2 >> tmp2)){
				tmp = 0;
			}
			variable x(tmp, tmp2, matrix);
			x.name = lineTokens[lineNum][1];
			mp[lineTokens[lineNum][1]] = x;
			cCode += x.name;
			//cout<<x.name<<endl;

			variables.push_back(x);
		}else{
			globalError = error("this is not an appropriate vector", lineNum);
			globalError.wrong = true;
			return false;
		}
		}
	}
	return true;
	}
}
bool isInitiallization(int lineNum){
	if(mp.count(lineTokens[lineNum][0]) != 1){
		globalError = error("this variable is not declared", lineNum);
		 globalError.wrong = true;
		return false;
	}else{
		if(mp[lineTokens[lineNum][0]].t == scalar){
				if(!isDouble(lineTokens[lineNum][2])){
					globalError = error("wrong initialization", lineNum);
					globalError.wrong = true;
					return false;
				}else{
					mp[lineTokens[lineNum][0]].values.push_back(lineTokens[lineNum][2]);
					int a = 2;
					isExpression(lineNum,a);
					myPop();
					token t = tokens.top();
					//cout<<lineTokens[lineNum][0]<<"[0][0] = "<<t.data<<endl;
					cCode += lineTokens[lineNum][0] + "[0][0] = " + t.data;
				}
		}else{
			if(lineTokens[lineNum].size()-4 == mp[lineTokens[lineNum][0]].dim1*mp[lineTokens[lineNum][0]].dim2){

				for(int i=0; i<mp[lineTokens[lineNum][0]].dim1; i++){
					for(int j=0; j<mp[lineTokens[lineNum][0]].dim2; j++){
						cCode += lineTokens[lineNum][0] + ".toMatrix[";
						//cout<<lineTokens[lineNum][0]<<".toMatrix[";
						cCode += to_string(i) + "][" + to_string(j) + "] =" + lineTokens[lineNum][i*mp[lineTokens[lineNum][0]].dim2+j+3] + ";\n";
						//cout<<i<<"]["<<j<<"] = "<<lineTokens[lineNum][i*mp[lineTokens[lineNum][0]].dim2+j+3]<<";"<<endl;
					}
				}
			}else{
				globalError = error("invalid initilaization", lineNum);
				globalError.wrong = true;
				return false;
			}
		}
		return true;
	}
}
void isExpression(int lineNum, int &tokenNum){
	//cout<<"//isExpression"<<endl;

	isTerm(lineNum,tokenNum);
	isMoreTerm(lineNum, tokenNum);
}

void printFunction(int lineNum){
	int tokenNum = 1;
	string temp = "";
	if(lineTokens[lineNum][tokenNum] == "("){
		tokenNum++;
		isExpression(lineNum, tokenNum);
		myPop();
		token exParam = tokens.top();
		tokens.pop();
		temp += "print(" + exParam.data + ");";
		cCode += temp + "\n";
		//cout<<temp<<endl;
	}else{
		globalError = error("function syntax error", lineNum);
		globalError.wrong = true;
		return;
	}
}

void assignment(int lineNum){

	int a = 2;
	token closingBraket;

	closingBraket.data = ");\n";
	tokens.push(closingBraket);

	isExpression(lineNum, a);
	myPop();
	//cout << tokens.top()._type<< " "<<mp[lineTokens[lineNum][0]].t<<endl;
	if(mp[lineTokens[lineNum][0]].dim1 != tokens.top().row || mp[lineTokens[lineNum][0]].dim2 != tokens.top().column){
		globalError = error("unmatched assignment", lineNum);
		globalError.wrong = true;
		return;
	}else{
		token equal;
		equal.data = "atama(";

		token leftHandSide;
		leftHandSide._type = mp[lineTokens[lineNum][0]].t;
		leftHandSide.column = mp[lineTokens[lineNum][0]].dim2;
		leftHandSide.row = mp[lineTokens[lineNum][0]].dim1;
		leftHandSide.data = lineTokens[lineNum][0];


		token coma;
		coma.data = ",";
		tokens.push(coma);
		tokens.push(leftHandSide);
		tokens.push(equal);


	while(!tokens.empty()){
		//cout<<tokens.top().data;
		cCode += tokens.top().data;
		tokens.pop();
	}
	cout<<endl;

	}
}

void statementFor(int lineNum){//when program face with a for in the first index of the line
	int currentLine;//statementFor() function will be executed
	int tokenNum = 1;
	bool nested = true;
	int a;
	string str = "";
	string str2 = "";
	if(lineTokens[lineNum][tokenNum] != "("){
		globalError = error("forloop syntax error", lineNum);
		globalError.wrong = true;
		return ;
	}else{
		tokenNum++;
		if(lineTokens[lineNum][3] == "in"){//if this is a nested loop
			nested = false;//the variable a is fixied for the appropriate index to find the first expression in the for loop
			a=4;//
			if(mp.count(lineTokens[lineNum][2]) != 1 ||  mp[lineTokens[lineNum][2]].t != scalar){
				globalError = error("forloop syntax error", lineNum);
				globalError.wrong = true;
				return ;
			}
		}else{
			if(lineTokens[lineNum][3] == ","){
				a=6;
				if(mp.count(lineTokens[lineNum][2]) != 1 || mp[lineTokens[lineNum][2]].t != scalar){
					globalError = error("forloop syntax error", lineNum);
					globalError.wrong = true;
					return ;
				}
			}else{
				globalError = error("comma is missing", lineNum);
				globalError.wrong = true;
				return ;
			}
		}
		isExpression(lineNum,a);
		myPop();
		token initial = tokens.top();
		tokens.pop();
		a++;
		isExpression(lineNum,a);
		myPop();
		token untill = tokens.top();
		tokens.pop();
		a++;
		isExpression(lineNum,a);
		myPop();
		token increment = tokens.top();
		tokens.pop();
		a++;
		str += "for(atama(" + lineTokens[lineNum][2] + "," + initial.data + ");varToNum(" + lineTokens[lineNum][2] + ")<=varToNum(" + untill.data + ");atama(" + lineTokens[lineNum][2] + ",addition(" + lineTokens[lineNum][2] + "," + increment.data + ")))";
		if(!nested){
			str += "{";
		}
		if(lineTokens[lineNum][3] == ","){
			a=12;

			isExpression(lineNum,a);
			myPop();
			token initial2 = tokens.top();
			tokens.pop();
			a++;
			isExpression(lineNum,a);
			myPop();
			token untill2 = tokens.top();
			tokens.pop();
			a++;
			isExpression(lineNum,a);
			myPop();
			token increment2 = tokens.top();
			tokens.pop();
			str2 += "for(atama(" + lineTokens[lineNum][4] + "," + initial2.data + ");varToNum(" + lineTokens[lineNum][4] + ")<=varToNum(" + untill2.data + ");atama("
					"" + lineTokens[lineNum][4] + ",addition(" + lineTokens[lineNum][4] + "," + increment2.data + "))){";

		}
		cCode += str + str2 + "\n";
		//cout<< str << endl <<str2<<endl;
	}
}

