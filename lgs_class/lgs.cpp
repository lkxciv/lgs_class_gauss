#include <iostream>
#include <string>

class LGS
{
private:
	int varsize;//x Anzahl der Variablen des lgs, Breite davon ist x+1
	int eqsize;//y Anzahl der Gleichungen, "Höhe" des lgs
	int varcount = 0; //Arbeitsvariable breite
	int eqcount = 0; //Arbeitsvariable höhe
	float **lgs; //Zwei ** wegen 2 dimensionen

public:
	//Anzahl der Variablen und Gleichungen sind gefordert
	//wichtig: wert an Adresse sollte von aussen nicht verändert werden
	LGS(int variable_num, int equation_num, float **matrix);

	~LGS();

	//Returnt gelöstes LGS in Stufenform
	LGS Ref();

	//Return LGS in vollständig gelöster Version, MUSS mit LGS in REF Form aufgerufen werden
	LGS OnlyRRef();

	//Returnt direkt von Normalform zu Ergebnisform
	LGS RRef();

	//Returnt String des LGS
	std::string ToString();

	//Prüft, ob lösung des LGS Punkt entspricht, LGS muss in RREf Form vorliegen
	bool IsSolutionPoint();

	//returnt Lösung in dyn. Float array, Länge ist varsize - 1
	float *GetSolution();
};

//methodendefinitionen von LGS

//Anzahl der Variablen und Gleichungen sind gefordert
//wichtig: Wert an Adresse sollte von aussen nicht verändert werden
LGS::LGS(int variable_num, int equation_num, float **matrix)
{
	//Variablen einstellen
	varsize = variable_num;
	eqsize = equation_num;
	varcount = varsize;
	eqcount = eqsize - 1;
	//Lgs zuweisen
	lgs = matrix;
}

LGS::~LGS()
{
	for (int y = 0; y <= eqsize; y++)
	{
		delete[] lgs[y];
	}
}

//Returnt gelöstes LGS in Stufenform
LGS LGS::Ref()
{
	int yzaehler; //Zaelvariable fuer berechnete Gleichungen
	int dgaenge; //Zaehlvariable für Rechendurchgaenge
	int ypos = 0;//y Richtungsdurchlauf
	int xpos = 0;//x Richtungsdurchlauf
	int elimx = 0;//ist beginn der treppendurchlauefe in x richtung, zaehlt die eliminationsvorgaenge der variablen
	int start_step = 0;//ist beginn der treppendurchlauefe in y richtung, bestimmt, ab welcher Gleichung weiter eliminiert wird
	float puffer = 0;
	LGS Lgs_ref(varsize, eqsize, lgs);//LGS, dass bearbeitet wird

																		//Zähler höchstens quadratisch machen
	if (eqsize > varsize)
	{
		yzaehler = varsize;
	}
	else if (varsize > eqsize || varsize == eqsize)
	{
		yzaehler = eqsize - 1;
	}

	start_step = 0;
	for (elimx = 0; elimx <= varsize; elimx++)//elimx ist beginn der treppendurchlauefe in x richtung, zaehlt die eliminationsvorgaenge der variablen
	{
		for (ypos = start_step; ypos < yzaehler; ypos++)//y Richtungsdurchlauf
		{
			puffer = Lgs_ref.lgs[ypos + 1][elimx];
			for (xpos = elimx; xpos <= varsize; xpos++)//x Richtungsdurchlauf
			{
				//Variable wir eliminiert oder mit El9minationsgleichung subtrahiert
				Lgs_ref.lgs[ypos + 1][xpos] = Lgs_ref.lgs[ypos + 1][xpos] - ((Lgs_ref.lgs[start_step][xpos] / Lgs_ref.lgs[start_step][elimx]) * puffer);
			}
		}
		start_step++;//start_step ist beginn der treppendurchlauefe in y richtung, bestimmt, ab welcher Gleichung weiter eliminiert wird
	}

	return Lgs_ref;
}

//Return LGS in vollständig gelöster Version, MUSS mit LGS in REF Form aufgerufen werden
LGS LGS::OnlyRRef()
{
	int yzaehler; //Zaelvariable fuer berechnete Gleichungen
	int dgaenge; //Zaehlvariable für Rechendurchgaenge
	int ypos = 0;//y Richtungsdurchlauf
	int xpos = 0;//x Richtungsdurchlauf
	LGS Lgs_rref(varsize, eqsize, lgs);//lgs sollte in ref form sein

																		 //Anfangs y-pos
	if (eqsize > varsize)
	{
		ypos = varsize;
	}
	else if (varsize > eqsize || varsize == eqsize)
	{
		ypos = eqsize - 1;
	}

	//LGS wird diagonal abgefahren von Unten rechts nach oben links
	for (xpos = varsize - 1; xpos >= 0; xpos--)
	{
		for (int varstep = xpos + 1; varstep < varsize; varstep++)
		{
			//Variable mit letztem Ergebnis multiplizieren, von Gleichungen subtrahieren
			Lgs_rref.lgs[ypos][varsize] -= Lgs_rref.lgs[ypos][varstep] * Lgs_rref.lgs[varstep][varsize];
			Lgs_rref.lgs[ypos][varstep] = 0;
		}
		//Gleichung durch Faktor von Var.
		Lgs_rref.lgs[ypos][varsize] /= Lgs_rref.lgs[ypos][xpos];
		Lgs_rref.lgs[ypos][xpos] = 1;
		ypos--;
	}

	return Lgs_rref;
}

//Returnt direkt von Normalform zu Ergebnisform
LGS LGS::RRef()
{
	LGS Solved(varsize, eqsize, lgs);
	Solved = Ref();
	Solved = OnlyRRef();
	return Solved;
}

//Returnt String des LGS
std::string LGS::ToString()
{
	std::string output = "";

	for (int row = 0; row <= eqcount; row++)//Schleife Zeilen
	{
		output += "[ ";
		for (int col = 0; col <= varcount; col++)//Schleife Spalten in einer Zeile
		{
			output += (std::to_string(lgs[row][col]) + " ");
		}
		output += "]\n";
	}

	return output;
}

bool LGS::IsSolutionPoint()
{
	//Prüfen ob es überhaupt lösenbar ist
	if (varsize > eqsize)
	{
		return 0;
	}
	//Diagonal abfahren
	for (int i = 0; i < varsize; i++)
	{
		//Es sollte immer 1 dort sein
		if (lgs[i][i] == 0)
		{
			return 0;
		}
	}

	return 1;
}

float *LGS::GetSolution()
{
	//Lösungsspeicher
	float *sol = new float[varsize];
	//Rechte Seite des LGS abfahren
	for (int i = 0; i < varsize; i++)
	{
		sol[i] = lgs[varsize][i];
	}

	return sol;
}