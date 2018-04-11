#include "Interpreter.h"

#include <fstream>
#include <algorithm>
using std::ifstream;

map<string, Var> vars;
int vCounter = 0, mCounter = 0;

string convShitTostring(String^ s) {
	const char* chars = (const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(s)).ToPointer();
	string dest = chars;
	return dest;
}

int getPiority(char op) {
	switch (op)
	{
	case '=':
		return 1;
	case '+':
		return 2;
	case '-':
		return 3;
	case '*':
		return 4;
	case '|':
		return 5;
	default:
		return -1;
	}
}


String^ getResultStr(String^ s) {
	string f = convShitTostring(s);
	Var result = ExecFormula(dealFormula(f));
	if (result.type == "Vector")
		return gcnew String(((Vector *)result.data)->ToString().c_str())+"\n";
	else if (result.type == "Matrix") {
		//other situation

		return gcnew String(((Matrix *)result.data)->ToString().c_str()) + "\n";
		
	}
	else if (result.type == "NumType") {
		return (*((NumType *)result.data)).ToString() + "\n";
	}
	else if (result.type == "Bool") {
		return *((bool*)result.data) ? "True" : "False";
	}
	// error
	return gcnew String(((string *)result.data)->c_str()) + "\n";
}

Vector* ToVector(void* p) {
	return (Vector*)p;
}

Matrix* ToMatrix(void* p) {
	return (Matrix*)p;
}

void loadVars(string path) {
	ifstream fin(path);
	int total;
	char type;
	fin >> total;
	string name;
	for (int i = 0; i < total; ++i) {
		fin >> type;
		Var var;
		int size;
		switch (type){
		case 'V':
			int dim;
			fin >> dim;
			var.type = "Vector";
			var.data = new Vector(dim);
			// var name
			name = "$v"+ std::to_string(vCounter++);
			// input
			for (int j = 0; j < dim; j++)
				fin >> ToVector(var.data)->data_[j];
			// add to map
			vars[name] = var;
			break;
		case 'M':
			int shape[2];
			fin >> shape[0] >> shape[1];
			var.type = "Matrix";
			var.data = new Matrix(shape[0], shape[1]);
			name = "$m" + std::to_string(mCounter++);
			size = shape[0] * shape[1];
			for (int j = 0; j < size; ++j)
				fin >> ToMatrix(var.data)->data_[j];
			vars[name] = var;
			break;
		default:
			break;
		}
	}
	fin.close();
}

string dealFormula(string f) {
	for (int i = 0; i < OpCMD.size(); ++i) {
		std::size_t find = f.find(OpCMD[i] + "(");
		while (find != string::npos) {
			f.replace(find, OpCMD[i].length(), OpCMD[i] + '|');
			find = f.find(OpCMD[i] + "(");
		}
	}
	return f;
}

int checkFormula(string f) {
	int status = NoError;
	// check if variable exist

	// check OpCMD is follow by (


	// check () is all right 

	return NoError;
}

string rmUseless(string f) {
	
	for (int i = 0; i < useless.size(); ++i)
	{
		
		f.erase(std::remove(f.begin(), f.end(), useless[i]), f.end());
	}
	return f;
}

string trimUseless(string f) {

	bool flag = true;
	for (int i = 0; i < f.size() && flag; ++i)
	{
		flag = false;
		for (int j = 0; j < useless.size(); ++j) {
			if (f[i] == useless[j]) {
				flag = true;
				f[i] = '@';
			}
		}
	}
	flag = true;
	for (int i = f.size()-1; i >= 0 && flag; --i)
	{
		flag = false;
		for (int j = 0; j < useless.size(); ++j) {
			if (f[i] == useless[j]) {
				flag = true;
				f[i] = '@';
			}
		}
	}
	f.erase(std::remove(f.begin(), f.end(), '@'), f.end());
	return f;
}

Var getVal(string f) {

	f = rmUseless(f);
	// is var
	for (auto it = vars.begin(); it != vars.end(); ++it) {
		if (it->first == f)
			return it->second;
	}
	
	// is value

	return Var{ "Num",new NumType(atof(f.c_str())) };

}

Var regularCale(Var a, Var b, char op) {
	Var result;
	switch (op)
	{
	case '+':
		if (a.type != b.type)
			break;
		if (a.type == "Vector") {
			result.data = new Vector();
			result.type = "Vector";
			try {
				*ToVector(result.data) = *ToVector(a.data) + *ToVector(b.data);
			}
			catch (const string e) {
				delete result.data;
				result.type = "Error";
				result.data = new string(e);
			}
			
		}
		else if (a.type == "Matrix") {
			result.data = new Matrix();
			result.type = "Matrix";
			try {
				*ToMatrix(result.data) = *ToMatrix(a.data) + *ToMatrix(b.data);
			}
			catch (const string e) {
				delete result.data;
				result.type = "Error";
				result.data = new string(e);
			}
		}
		break;
	case '-':
		if (a.type != b.type)
			break;
		if (a.type == "Vector") {
			result.data = new Vector();
			result.type = "Vector";
			try {
				*ToVector(result.data) = *ToVector(a.data) - *ToVector(b.data);
			}
			catch (const string e) {
				delete result.data;
				result.type = "Error";
				result.data = new string(e);
			}

		}
		else if (a.type == "Matrix") {
			result.data = new Matrix();
			result.type = "Matrix";
			try {
				*ToMatrix(result.data) = *ToMatrix(a.data) - *ToMatrix(b.data);
			}
			catch (const string e) {
				delete result.data;
				result.type = "Error";
				result.data = new string(e);
			}
		}
		break;
	case '*':
		if (a.type != b.type)
			break;
		if (a.type == "Vector") {
			Vector *ap = ToVector(a.data), *bp = ToVector(b.data),*tmp;
			if (ap->data_.size() != 1 && bp->data_.size() != 1) {
				result.type = "Error";
				result.data = new string("Vector only have scalar *");
			}
			// swap
			if (bp->data_.size() == 1) {
				tmp = ap;
				ap = bp;
				bp = tmp;
			}
			result.type = "Vector";
			result.data = new Vector();
			try {
				*ToVector(result.data) = bp->Scalar(ap->data_[0]);
			}
			catch (const string e) {
				delete result.data;
				result.type = "Error";
				result.data = new string(e);
			}
		}
		else if (a.type == "Matrix") {
			result.data = new Matrix();
			result.type = "Matrix";
			try {
				*ToMatrix(result.data) = *ToMatrix(a.data) * *ToMatrix(b.data);
			}
			catch (const string e) {
				delete result.data;
				result.type = "Error";
				result.data = new string(e);
			}
		}
		break;
	// etc
	default:
		result.type = "Error";
		result.data = new string("operator is not define");
		break;
	}
	return result;
}

Var funcCale(string cmd, string argument) {
	vector<Var> args;
	cmd = rmUseless(cmd);
	argument = trimUseless(argument) + ",";
	// deal right side
	Var result;
	size_t commaPosi = argument.find(","), lastPosi = 0;
	
	while(commaPosi != string::npos){
		args.push_back(ExecFormula(argument.substr(lastPosi, commaPosi - lastPosi)));
		lastPosi = commaPosi + 1;
		commaPosi = argument.find(",",lastPosi);
	}


	// cale
	if (cmd == "Dot") {
		// check arguments
		if (args.size() != 2 || args[0].type != "Vector" || args[1].type != "Vector")
			return Var{ "Error",new string("parameter wrong") };
		// dot it
		result.type = "NumType";
		result.data = new NumType();
		try {
			*((NumType *)result.data) = ToVector(args[0].data)->Dot(*ToVector(args[1].data));
		}
		catch (const string e) {
			delete result.data;
			result.type = "Error";
			result.data = new string(e);
		}
		return result;
	}
	else if (cmd == "Norm") {
		// check argument
		if(args.size() != 1 || args[0].type != "Vector")
			return Var{ "Error",new string("parameter wrong") };
		result.type = "NumType";
		result.data = new NumType();
		try {
			*((NumType *)result.data) = ToVector(args[0].data)->Norm();
		}
		catch (const string e) {
			delete result.data;
			result.type = "Error";
			result.data = new string(e);
		}
		return result;
	}
	else if(cmd == "Normli") {
		if (args.size() != 1 || args[0].type != "Vector")
			return Var{ "Error",new string("parameter wrong") };
		result.type = "Vector";
		result.data = new Vector();
		try {
			*ToVector(result.data) = ToVector(args[0].data)->Normalization();
		}
		catch (const string e) {
			delete result.data;
			result.type = "Error";
			result.data = new string(e);
		}
		
		return result;
	}
	else if (cmd == "Cross") {
		// check arguments
		if (args.size() != 2 || args[0].type != "Vector" || args[1].type != "Vector")
			return Var{ "Error",new string("parameter wrong") };
		// 
		result.type = "Vector";
		result.data = new Vector();;
		try {

			*ToVector(result.data) = ToVector(args[0].data)->Cross(*ToVector(args[1].data));
		}
		catch (const string e) {
			delete result.data;
			result.type = "Error";
			result.data = new string(e);
		}
		return result;
	} // below need test
	else if (cmd == "Compo") {
		// check arguments
		if (args.size() != 2 || args[0].type != "Vector" || args[1].type != "Vector")
			return Var{ "Error",new string("parameter wrong") };
		// 
		result.type = "NumType";
		result.data = new NumType();
		try {
			*((NumType *)result.data) = ToVector(args[0].data)->Component(*ToVector(args[1].data));
		}
		catch (const string e) {
			delete result.data;
			result.type = "Error";
			result.data = new string(e);
		}
		return result;
	}
	else if (cmd == "Proj") {
		// check arguments
		if (args.size() != 2 || args[0].type != "Vector" || args[1].type != "Vector")
			return Var{ "Error",new string("parameter wrong") };
		// 
		result.type = "Vector";
		result.data = new Vector();;
		try {

			*ToVector(result.data) = ToVector(args[0].data)->Projection(*ToVector(args[1].data));
		}
		catch (const string e) {
			delete result.data;
			result.type = "Error";
			result.data = new string(e);
		}
		return result;
	}
	else if (cmd == "Tri") {
		// check arguments
		if (args.size() != 2 || args[0].type != "Vector" || args[1].type != "Vector")
			return Var{ "Error",new string("parameter wrong") };
		// 
		result.type = "NumType";
		result.data = new NumType();
		try {
			*((NumType *)result.data) = ToVector(args[0].data)->TriangleArea(*ToVector(args[1].data));
		}
		catch (const string e) {
			delete result.data;
			result.type = "Error";
			result.data = new string(e);
		}
		return result;
	}
	else if (cmd == "Paral") {
		// check arguments
		if (args.size() != 2 || args[0].type != "Vector" || args[1].type != "Vector")
			return Var{ "Error",new string("parameter wrong") };
		// 
		result.type = "Bool";
		result.data = new bool();
		try {
			*((bool *)result.data) = ToVector(args[0].data)->Parallel(*ToVector(args[1].data));
		}
		catch (const string e) {
			delete result.data;
			result.type = "Error";
			result.data = new string(e);
		}
		return result;
	}
	else if (cmd == "Ortho") {
		// check arguments
		if (args.size() != 2 || args[0].type != "Vector" || args[1].type != "Vector")
			return Var{ "Error",new string("parameter wrong") };
		// 
		result.type = "Bool";
		result.data = new bool();
		try {
			*((bool *)result.data) = ToVector(args[0].data)->Orthogonal(*ToVector(args[1].data));
		}
		catch (const string e) {
			delete result.data;
			result.type = "Error";
			result.data = new string(e);
		}
		return result;
	}
	else if (cmd == "Angle") {
		// check arguments
		if (args.size() != 2 || args[0].type != "Vector" || args[1].type != "Vector")
			return Var{ "Error",new string("parameter wrong") };
		// 
		result.type = "NumType";
		result.data = new NumType();
		try {
			*((NumType *)result.data) = ToVector(args[0].data)->Getangle(*ToVector(args[1].data));
		}
		catch (const string e) {
			delete result.data;
			result.type = "Error";
			result.data = new string(e);
		}
		return result;
	}
	else if (cmd == "PlaneNorm") {
		// check arguments
		if (args.size() != 2 || args[0].type != "Vector" || args[1].type != "Vector")
			return Var{ "Error",new string("parameter wrong") };
		// 
		result.type = "Vector";
		result.data = new Vector();;
		try {

			*ToVector(result.data) = ToVector(args[0].data)->PlaneNormal(*ToVector(args[1].data));
		}
		catch (const string e) {
			delete result.data;
			result.type = "Error";
			result.data = new string(e);
		}
		return result;
	}
	else if (cmd == "Indepen") {
		// check arguments
		if(args.size()<2)
			return Var{ "Error",new string("parameter wrong") };
		for (int i = 0; i < args.size(); ++i) {
			if(args[i].type != "Vector")
				return Var{ "Error",new string("parameter wrong") };
		}
		//
		result.type = "Bool";
		result.data = new bool(true);
		try{
			for (int i = 0; i < args.size(); ++i) {
				for (int j = i; j < args.size(); ++j) {
					*((bool *)result.data) &= ToVector(args[i].data)->LinearIndependent(*ToVector(args[j].data));
				}
			}
		}
		catch (const string e) {
			delete result.data;
			result.type = "Error";
			result.data = new string(e);
		}
		return result;
	}
	else if (cmd == "GramS") {
		valarray<Vector> basis(args.size());
		// check arguments
		if (args.size()<2)
			return Var{ "Error",new string("parameter wrong") };
		for (int i = 0; i < args.size(); ++i) {
			basis[i] = *ToVector(args[i].data);
			if (args[i].type != "Vector")
				return Var{ "Error",new string("parameter wrong") };
		}
		//
		result.type = "Array";
		result.data = new valarray<Vector>(args.size());
		try {
			*((valarray<Vector>*)result.data) = 
		}
		catch (const string e) {
			result.type = "Error";
			result.data = new string(e);
		}
	}
	return Var{ "Error",new string("CMD is wrong") };
}


Var ExecFormula(string f) {
	int is_error = checkFormula(f);
	if (is_error != NoError)
		return Var{ "Error", new int(is_error) };
	int baseVal = 0, opCount = 0, minLoc = -1, minVal = INF, pio;

	for (int i = 0; i<f.length(); ++i) {
		if (f[i] == '(') {
			baseVal += 6;
		}
		else if (f[i] == ')') {
			baseVal -= 6;
		}
		else {
			pio = getPiority(f[i]);
			if (pio != -1) {
				++opCount;
				if (pio + baseVal <= minVal) {
					minLoc = i;
					minVal = pio + baseVal;
				}
			}
		}
	}
	if (!opCount) {
		return getVal(f);
	}
	else {
		if (f[minLoc] == '|') {
			return funcCale(f.substr(0, minLoc), f.substr(minLoc + 1, f.length() - minLoc));
		}
		else if (f[minLoc] == '=') {
			Var result = ExecFormula(f.substr(minLoc + 1, f.length() - minLoc));
			
			// set value to var map

			vars[rmUseless(f.substr(0, minLoc))] = result;
			

			return result;

			
		}
		else {
			return regularCale(ExecFormula(f.substr(0, minLoc)), ExecFormula(f.substr(minLoc + 1, f.length() - minLoc)), f[minLoc]);
			
		}
	}
}
