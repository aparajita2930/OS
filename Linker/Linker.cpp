#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <vector>
using namespace std;

typedef struct Symbol
{
	string sym_name;
	int val;
	bool defined_prev;
	bool sym_in_useList;
	int moduleNumber;
}symbol;

typedef struct
{
	int defcount;
	string sym;
	int address_offset;
}DefList;

typedef struct
{
	int usecount;
	string use_sym;
}UseList;

typedef struct
{
	string instr_type;
	int instr_val;
}Instruction;

typedef struct
{
	string token;
	int line_num, line_offset;
}Token;

typedef struct
{
	DefList def_list;
	UseList use_list;
	Instruction instr;
	int base_address, module_id, module_length, sym_count;
} Mod;
int line_number = 1, column_offset = 0, prev_column_offset = 1, module_num = 0, word_count = 0, object_type = 1, symbol_count = 0;
Symbol *sym = new Symbol[512];
int memOffset= 0;
string errMsg="", token = "";

void parseerror(int lineno, int column_off, int errcode) {
static string errstr[] = {
"NUM_EXPECTED", // Number expect
"SYM_EXPECTED", // Symbol Expected
"ADDR_EXPECTED", // Addressing Expected
"SYM_TOLONG", // Symbol Name is to long
"TO_MANY_DEF_IN_MODULE", // > 16
"TO_MANY_USE_IN_MODULE", // > 16
"TO_MANY_INSTR" // total num_instr exceeds memory size (512)
};
//cout << lineno << column_off << prev_column_offset << token.length() << endl;
if (column_off == 0)
{
	line_number--;
	column_off = prev_column_offset;
	column_off = token.empty() ? column_off : column_off - token.length();
}
column_off = column_off == 0 ? 1 : column_off;
cout << "Parse Error line "<<line_number<<" offset "<<column_off<<": "<< errstr[errcode] <<endl;
}

bool chkOpCode(string instr)
{
	for (unsigned int i = 0; i < instr.length(); i++)
	{
		if (!(instr[i] >= '0' && instr[i] <= '9') || instr.length()>4)
		{
			return false;
		}
	}
	return true;
}

bool chk_num_expected(string instr)
{
	for (unsigned int i = 0; i < instr.length(); i++)
	{
		if (!(instr[i] >= '0' && instr[i] <= '9'))
		{
			return false;
		}
	}
	return true;
}

bool chk_symbol_expected(string instr)
{
	if (!isalpha(instr[0]))
	{
		return false;
	}

	for (unsigned int i = 0; i < instr.length(); i++)
	{
		if (!isalnum(instr[i]))
		{
			return false;
		}
	}
	return true;
}

bool chk_addr_expected(string instr)
{
	if (instr.length() > 1)
	{
		return false;
	}
	else if (instr[0] != 'I' && instr[0] != 'A' && instr[0] != 'R' && instr[0] != 'E')
	{
		return false;
	}
	return true;
}

/**************Pass one***********/
void first_pass(char** argv) {
	string str;
	int defcount = 0, usecount = 0, instrCount = 0, i = 0;
	//int instrListCounter = 0;
	bool isSwitch = true;
	bool isComplete = true;
	string symbol = "";
	ifstream readInput(argv[1]);
	if (!readInput.is_open()) {
		cout << "The Input file could not be opened for read.";
	} else {
		char c;
		token = "";
		while (readInput.get(c)) {

			//cout << c << " " << column_offset << endl;
			while (c != ' ' && c != '\t' && c != '\n') {

				token = token + c;
				column_offset++;
				//cout << "while c = " << c << "column_offset: " << column_offset << endl;
				readInput.get(c);
			}

			if (c == ' ' || c == '\t' || c == '\n') {
				column_offset++;
				//cout << "c = " << c << " column_offset: " << column_offset << endl;
				if (!token.empty()) {
					//cout << "|"<< token  << " | line offset : "<< line_number << "column_offset:" << column_offset <<" | column_offset : " << column_offset - token.length() << " | word count : " << word_count << " | object_type : " << object_type << endl;
					word_count++;

					switch (object_type) {
					case 1:
					{
						string deflist_count;
						if (isSwitch) {
							defcount = 2 * atoi(token.c_str());
							//cout << "defcount(*2): " << defcount << endl;
							isComplete = false;
							deflist_count = token;
							//object_type = 1;
							isSwitch = false;
							if (defcount == 0) {
								//cout << "defcount-0: " << defcount << endl;
								word_count = 1;
								object_type = 2;
								isSwitch = true;
							}
							//cout << "line_number: " << line_number << " column_offset: " << column_offset << endl;
							bool defcount_check = chk_num_expected(deflist_count);
							if (!defcount_check)
							{
								parseerror(line_number,column_offset-token.length(),0);
								//parseerror(line_number,column_offset,0);
								exit(1);
							}
							else if (atoi(deflist_count.c_str()) > 16)
							{
								//cout << "line_number: " << line_number << " column_offset: " << column_offset << endl;
								parseerror(line_number,column_offset-token.length(),4);
								//parseerror(line_number,column_offset,4);
								exit(1);
							}
						} else {
							if (defcount % 2 == 0) {
								//cout << "defcountmod2-0: " << defcount << endl;
								symbol = token;
								bool def_symbol_check = chk_symbol_expected(symbol);
								//cout << "token " << token << endl;
								//cout << "line_number: " << line_number << " column_offset: " << column_offset << endl;
								if (!def_symbol_check)
								{
									parseerror(line_number,column_offset-token.length(),1);
									//parseerror(line_number,column_offset,1);
									exit(1);
								}
								else if (symbol.length()>16)
								{
									parseerror(line_number,column_offset-token.length(),3);
									//parseerror(line_number,column_offset,3);
									exit(1);
								}

								//cout << "symbol: " << symbol << endl;
							} else {
								//cout << "defcountmod2-1: " << defcount << endl;
								int mem = atoi(token.c_str()) + memOffset;
								//cout << "memOffset: " << token << endl;
								bool def_offset_check = chk_num_expected(token);
								//cout << "token " << token << endl;
								if (!def_offset_check)
								{
									parseerror(line_number,column_offset-token.length(),0);
									//parseerror(line_number,column_offset,0);
									exit(1);
								}
								bool isDefinedBefore = false;
								for (i = 0; i < symbol_count; i++) {
									if (sym[i].sym_name == symbol) {
										sym[i].defined_prev = true;
										isDefinedBefore = true;
										break;
									}
								}
								if (!isDefinedBefore) {

									sym[symbol_count].sym_name = symbol;
									sym[symbol_count].val = mem;
									sym[symbol_count].defined_prev = false;
									sym[symbol_count].sym_in_useList = false;
									sym[symbol_count].moduleNumber = module_num+1;
									symbol_count++;
								}
							}
							if (defcount == 1) {
								//cout << "defcount-1: " << defcount << endl;
								word_count = 1;
								object_type = 2;
								isSwitch = true;
								defcount--;
							} else {
								defcount--;
								//cout << "defcount--: " << defcount << endl;
							}
						}
						break;
					}
					case 2:
					{
						string uselist_count;
						int useListCounter = 0;
						if (isSwitch) {
							usecount = atoi(token.c_str());
							useListCounter = usecount;
							uselist_count = token;
							isSwitch = false;
							if (usecount == 0) {
								word_count = 1;
								object_type = 3;
								isSwitch = true;
							}

							bool usecount_check = chk_num_expected(uselist_count);
							if (!usecount_check)
							{
								parseerror(line_number,column_offset-token.length(),0);
								exit(1);
							}
							else if (atoi(uselist_count.c_str()) > 16)
							{
								parseerror(line_number,column_offset-token.length(),5);
								exit(1);
							}
						}
						else{
							//cout << token << endl;
							bool use_symbol_check = chk_symbol_expected(token);
							//cout << "token " << token << endl;
							//cout << "line_number: " << line_number << " column_offset: " << column_offset << endl;
							if (!use_symbol_check)
							{
								parseerror(line_number,column_offset-token.length(),1);
								exit(1);
							}
							else if (token.length()>16)
							{
								parseerror(line_number,column_offset-token.length(),3);
								exit(1);
							}
							useListCounter--;
						}
						if (usecount == 0) {
							object_type = 3;
							word_count = 0;
							isSwitch = true;
						} else {
							usecount--;

						}

						break;
					}
					case 3:
					{
						string instrlist_count;
						if (isSwitch) {
							int mem = atoi(token.c_str());
							//instrListCounter = 2*mem;
							instrlist_count = token;
							//cout << "codecount: " << instrlist_count << endl;
							// Rule 5
							for (i = 0; i < symbol_count; i++) {

								if(sym[i].val - memOffset >= mem && module_num == sym[i].moduleNumber - 1){
									//cout << "memOffset : " << memOffset << " mem: " << mem << endl;
									cout << "Warning: Module "<< sym[i].moduleNumber<< ": " << sym[i].sym_name << " to big " << sym[i].val - memOffset  <<" (max="<<mem - 1<<") assume zero relative" << endl;
									sym[i].val= memOffset;
								}
							}

							instrCount = 2 * mem;
							isSwitch = false;
							if (instrCount == 0) {
								word_count = 1;
								object_type = 1;
								isSwitch = true;
							}
							bool instrcount_check = chk_num_expected(instrlist_count);
							//cout << "instrlist " << instrlist_count << " instrcheck " << instrcount_check << endl;
							if (!instrcount_check)
							{
								parseerror(line_number,column_offset-token.length(),0);
								exit(1);
							}
							else if (mem + memOffset > 512)
							{
								parseerror(line_number,column_offset-token.length(),6);
								exit(1);
							}
							memOffset += mem;
						}
						else
						{
							//cout << "instr: " << instrCount << endl;
							if (instrCount%2==1)
							{
								//cout << "instreven: " << instrCount << endl;
								bool instr_address_check = chk_addr_expected(token);
								//cout << "token1 " << token << endl;
								if (!instr_address_check)
								{
									parseerror(line_number,column_offset-token.length(),2);
									exit(1);
								}

							}
							else if (instrCount%2==0)
							{
								//cout << "instrodd: " << instrCount << endl;
								bool instr_count_check = chk_num_expected(token);
								//cout << "token0 " << token << endl;
								if (!instr_count_check)
								{
									parseerror(line_number,column_offset-token.length(),0);
									exit(1);
								}


							}
							//instrListCounter--;
						}
						if (instrCount <= 0) {
							object_type = 1;
							word_count = 0;
							module_num++;
							isComplete = true;
							isSwitch = true;
						} else {

						}
						instrCount--;
						break;
					}
					}
				}
				token.clear();
				if (c == '\n')
				{
					prev_column_offset = column_offset;
					column_offset = 0;
					line_number++;
					//cout << "prev_column_offset: " << prev_column_offset << endl;
				}
				//column_offset++;
			}
		}

		//cout << "iscomplete " << isComplete << endl;
		if (isComplete==false)
		{
			//cout << "complete " << defcount << endl;
			//cout << "instrcount " << instrCount << endl;
			//cout << "line_number: " << line_number << " column_offset: " << column_offset << endl;
			if (defcount!=0 && defcount%2==0)
			{
				//cout << "symbol: " << defcount << " line_number: " << line_number << " column_offset: " << column_offset << " prev_column_offset: " << prev_column_offset << endl;
				parseerror(line_number,column_offset,1);
				//cout << "symbol: " << defcount << " line_number: " << line_number << " column_offset: " << column_offset << " prev_column_offset: " << prev_column_offset << endl;
				exit(1);
			}
			else if (defcount!=0 && defcount%2==1)
			{
				//cout << "symbol: " << defcount;
				parseerror(line_number,column_offset,0);
				exit(1);
			}

			if (usecount!=0)
			{
				parseerror(line_number,column_offset,1);
				exit(1);
			}

			//cout << "instrcount " << instrCount << endl;
			if (instrCount!=0 && instrCount%2==0)
			{
				parseerror(line_number,column_offset,0);
				exit(1);
			}
			else if (instrCount!=0 && instrCount%2==1)
			{
				parseerror(line_number,column_offset,2);
				exit(1);
			}
		}
	}
}

/**************Pass two***********/
void second_pass(char** argv)
{
	string str;
	int defcount = 0, usecount = 0, instrCount = 0, i = 0, mem_address = 0, usecountFixed = 0;
	int memoryMapCount = 0;
	bool isSwitch = true;
	module_num = 1;
	vector<string> useList;
	memOffset = 0;
	cout << endl << "Memory Map" << endl;
	string symbol = "";
	ifstream readInput(argv[1]);
	if (!readInput.is_open())
	{
		cout << "The Input file could not be opened for read.";
	}
	else
	{
		char c;
		token = "";
		vector<string> e_vector;
		while (readInput.get(c))
		{
			while (c != ' ' && c != '\t' && c != '\n')
			{
				column_offset++;
				token = token + c;
				readInput.get(c);
			}
			if (c == ' ' || c == '\t' || c == '\n')
			{
				if (!token.empty())
				{
					word_count++;
					//cout << token << " | line offset : "<< line_number << " | column_offset : " << column_offset - token.length() << " | word count : " << word_count << " | object_type : " << object_type << endl;
					switch (object_type)
					{
					case 1:
					{
						if (isSwitch)
						{
							defcount = 2 * atoi(token.c_str());
							//object_type = 1;
							isSwitch = false;
							if (defcount == 0)
							{
								word_count = 1;
								object_type = 2;
								isSwitch = true;
							}
						}
						else
						{
							if (defcount % 2 == 0)
							{
								symbol = token;
							}
							else
							{
								//int mem = atoi(token.c_str()) + memOffset;

							}
							if (defcount == 1)
							{
								word_count = 1;
								object_type = 2;
								isSwitch = true;
							}
							else
							{
								defcount--;
							}
						}
						break;
					}
					case 2:
					{
						//bool is_sym_in_useList = false;
						if (isSwitch)
						{
							usecount = atoi(token.c_str());
							usecountFixed = usecount;
							isSwitch = false;
							word_count = 0;
							if (usecount == 0)
							{

								word_count = 1;
								object_type = 3;
								isSwitch = true;
							}
						}
						else
						{
						if (usecount == 1)
						{
							object_type = 3;
							word_count = 0;
							isSwitch = true;
							useList.push_back(token);
							for (i = 0; i < symbol_count; i++)
							{
								if (sym[i].sym_name == token)
								{
									sym[i].sym_in_useList = true;
									//is_sym_in_useList = true;
									break;
								}
							}
						}
						else
						{
							if(word_count != 0)
							{
								useList.push_back(token);
							}


							for (i = 0; i < symbol_count; i++)
							{
								if (sym[i].sym_name == token)
								{

									sym[i].sym_in_useList = true;
									//is_sym_in_useList = true;
									break;
								}
							}

						}
						usecount--;
						}
						break;
					}
					case 3:
					{
						int mem = 0;
						if (isSwitch)
						{
							mem_address = atoi(token.c_str());
							instrCount = 2 * mem_address;
							isSwitch = false;
							if (instrCount == 0)
							{
								word_count = 1;
								object_type = 1;
								isSwitch = true;
							}

						}
						else
						{

							if (instrCount % 2 == 1)
							{
								symbol = token;
							}
							else
							{
								mem = atoi(token.c_str());
								if(symbol == "I")
								{
									bool opcode_err = chkOpCode(token);
									if (!opcode_err)
									{
										errMsg = "9999 Error: Illegal immediate value; treated as 9999";
										cout << setfill('0') << setw(3) << memoryMapCount << ": " << errMsg << endl;
									}
									else
									{
										errMsg = "";
										cout << setfill('0') << setw(3) << memoryMapCount << ": " << setfill('0') << setw(4) << mem << errMsg << endl;
									}
								}
								else if (symbol == "A")
								{
									bool opcode_err = chkOpCode(token);
									if (!opcode_err)
									{
										errMsg = "9999 Error: Illegal opcode; treated as 9999";
										cout << setfill('0') << setw(3) << memoryMapCount << ": " << errMsg << endl;
									}
									else if (token.length()==4 && atoi(token.substr(1).c_str())>511)
									{
										errMsg = "000 Error: Absolute address exceeds machine size; zero used";
										cout << setfill('0') << setw(3) << memoryMapCount << ": " << token.substr(0,1) << errMsg << endl;
									}
									else
									{
										errMsg = "";
										cout << setfill('0') << setw(3) << memoryMapCount << ": " << setfill('0') << setw(4) << mem << errMsg << endl;
									}
								}
								else if(symbol == "R")
								{
									mem += memOffset;
									bool opcode_err = chkOpCode(token);
									if (!opcode_err)
									{
										errMsg = "9999 Error: Illegal opcode; treated as 9999";
										cout << setfill('0') << setw(3) << memoryMapCount << ": " << errMsg << endl;
									}
									else if (token.length()==4 && atoi(token.substr(1).c_str())>mem_address)
									{
										errMsg = " Error: Relative address exceeds module size; zero used";
										cout << setfill('0') << setw(3) << memoryMapCount << ": " << (atoi(token.substr(0,1).c_str())*1000)+memOffset << errMsg << endl;
									}
									else
									{
										errMsg = "";
										cout << setfill('0') << setw(3) << memoryMapCount << ": " << setfill('0') << setw(4) << mem << errMsg << endl;
									}
								}
								else if(symbol == "E")
								{

									string useListIndex = token.substr(1,3);
									unsigned int index = atoi(useListIndex.c_str());
									bool opcode_err = chkOpCode(token);
									//cout << "token: " << token << endl;
									if (!opcode_err)
									{
										errMsg = "9999 Error: Illegal opcode; treated as 9999";
										cout << setfill('0') << setw(3) << memoryMapCount << ": " << errMsg << endl;
									}
									else if(index >= useList.size())
									{
										errMsg = " Error: External address exceeds length of uselist; treated as immediate";
										cout << setfill('0') << setw(3) << memoryMapCount << ": " << setfill('0') << setw(4) << mem << errMsg << endl;

									}
									else
									{
									mem = 0;
									string usedVar = useList.at(index);
									//bool in_uselist_evector = true;
									e_vector.push_back(usedVar);
									for(int i = 0; i < symbol_count; i++)
									{
										if(sym[i].sym_name == usedVar)
										{
											mem = sym[i].val + atoi((token.substr(0,1) + "000").c_str());
										}
									}

									if(mem == 0)
									{
										errMsg = " Error: "+usedVar+" is not defined; zero used";
										mem = atoi((token.substr(0,1) + "000").c_str());
										cout << setfill('0') << setw(3) << memoryMapCount << ": " << setfill('0') << setw(4) << mem << errMsg << endl;

									}
									else{
										bool opcode_err = chkOpCode(token);
										if (!opcode_err)
										{
											errMsg = "9999 Error: Illegal opcode; treated as 9999";
											cout << setfill('0') << setw(3) << memoryMapCount << ": " << errMsg << endl;
										}
										else if (token.length()==4 && atoi(token.substr(1).c_str())>usecountFixed)
										{
											errMsg = " Error: External address exceeds length of uselist; treated as immediate";
											cout << setfill('0') << setw(3) << memoryMapCount << ": " << setfill('0') << setw(4) << mem << errMsg << endl;
										}
										else
										{
											errMsg = "";
											cout << setfill('0') << setw(3) << memoryMapCount << ": " << setfill('0') << setw(4) << mem << errMsg << endl;
										}
									}
									}
								}
								//cout << setfill('0') << setw(3) << memoryMapCount << ": " << mem << errMsg << endl;
								memoryMapCount++;

							}
						}
						if (instrCount <= 0)
						{
							object_type = 1;
							word_count = 0;
							memOffset += mem_address;
							isSwitch = true;
							bool found = false;
							//cout  << "useList.size() : "<< useList.size() << " : e_vector.size() : " << e_vector.size() << " | module_num " << module_num << endl;
							for (unsigned int i = 0; i < useList.size(); i++)
							{

								for (unsigned int j = 0; j < e_vector.size(); j++)
								{
									//cout << "uselist " << useList[i] << " evector " << e_vector[j] << endl;
									if (useList[i]==e_vector[j])
									{
										//cout << "uselist " << useList[i] << " evector " << e_vector[j] << " true " << endl;
										found = true;
										break;
									}
								}

								if(!found){
									//cout << "i: " << i << " sym[i] " << sym[i].sym_name << " in_uselist: " << sym[i].sym_in_useList << endl;
									sym[i].sym_in_useList = false;
									cout << "Warning: Module " << module_num << ": " << useList[i] << " appeared in the uselist but was not actually used" << endl;
								}
							}
							module_num++;
							useList.clear();
							e_vector.clear();
						}
						else
						{
							instrCount--;
						}
						break;
					}
					}
				}
				token.clear();
				if (c == '\n')
				{
					column_offset = 0;
					line_number++;
				}
				column_offset++;
			}
		}
	}
}

void printSymbols()
{
	cout << "Symbol Table" << endl;
	for(int i = 0; i < symbol_count; i++)
	{
		if(sym[i].defined_prev)
		{
			cout << sym[i].sym_name <<"=" << sym[i].val << " Error: This variable is multiple times defined; first value used" <<endl;
		}
		else
		{
			cout << sym[i].sym_name <<"=" << sym[i].val<< endl;
		}

	}
}

//vector<Symbol> sym;
int main(int argc, char **argv)
{
	if (argc == 2)
	{
		/**************call pass one***********/
		first_pass(argv);
		printSymbols();
		/**************call pass two***********/
		second_pass(argv);
		cout << endl;
		for (int i = 0; i < symbol_count; i++)
		{
			if (!sym[i].sym_in_useList)
			{
				cout << "Warning: Module " << sym[i].moduleNumber << ": " << sym[i].sym_name << " was defined but never used" << endl;
			}
		}

	}
	else
	{
		cout << "Invalid number of arguments. Please give the input file to be used along with its path.";
	}

	//std::cin.get();
}

