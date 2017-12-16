#include<bits/stdc++.h>
using namespace std;
#define NUMBER_OF_INTERVALS 5

fstream h("valtoattr.txt",ios::out);
int counter = 1;
void process_continuous(vector<double> v, vector<int> &classes, int numintervals,string attrname)
{
	double max, min;
	max = *max_element(v.begin(),v.end());
	min = *min_element(v.begin(),v.end());	
	double intervalwidth = (max-min)/numintervals;
	for (int i = 0; i < numintervals; ++i)
	{
		h<<counter<<" "<<(min+(i*numintervals))<<"<="<<attrname<<"<"<<(min+((i+1)*numintervals))<<endl;
		counter++;
	}
	for (int i = 0; i < v.size(); ++i)
	{
		int cl = ceil((v[i] - min)/intervalwidth);
		if(cl==0)
			cl = 1;
		classes.push_back(cl);
	}
}

int main()
{
  //  Attributes
  //  1. Number of times pregnant
  //  2. Plasma glucose concentration a 2 hours in an oral glucose tolerance test
  //  3. Diastolic blood pressure (mm Hg)
  //  4. Triceps skin fold thickness (mm)
  //  5. 2-Hour serum insulin (mu U/ml)
  //  6. Body mass index (weight in kg/(height in m)^2)
  //  7. Diabetes pedigree function
  //  8. Age (years)
  //  9. Class variable (0 or 1)

	fstream f("pima-indians-diabetes.data", ios::in);
	fstream g("transactions.txt", ios::out);
	int numattributes = 9;
	int numdata = 768;

	vector<double> npreg(numdata);
	vector<double> gluc(numdata);
	vector<double> bp(numdata);
	vector<double> triceps(numdata);
	vector<double> si(numdata);
	vector<double> bmi(numdata);
	vector<double> dpf(numdata);
	vector<double> age(numdata);
	vector<int> hasdm(numdata);

	vector<vector<int> > itemsets;

	for (int i = 0; i < numdata; ++i)
	{

		f>>npreg[i]>>gluc[i]>>bp[i]>>triceps[i]>>si[i]>>bmi[i]>>dpf[i]>>age[i]>>hasdm[i];
		hasdm[i]++;
	}

	vector<int> temp;
	process_continuous(npreg,temp,NUMBER_OF_INTERVALS,"number-of-pregnancies");
	itemsets.push_back(temp);	
	temp.clear();
	process_continuous(gluc,temp,NUMBER_OF_INTERVALS,"plasma-glucose-concentration");
	itemsets.push_back(temp);
	temp.clear();
	process_continuous(bp,temp,NUMBER_OF_INTERVALS,"diastolic-blood-pressure");
	itemsets.push_back(temp);
	temp.clear();
	process_continuous(triceps,temp,NUMBER_OF_INTERVALS,"triceps-skin-fold-thickness");
	itemsets.push_back(temp);
	temp.clear();
	process_continuous(si,temp,NUMBER_OF_INTERVALS,"serum-insulin");
	itemsets.push_back(temp);
	temp.clear();
	process_continuous(bmi,temp,NUMBER_OF_INTERVALS,"body-mass-index");
	itemsets.push_back(temp);
	temp.clear();
	process_continuous(dpf,temp,NUMBER_OF_INTERVALS,"diabetes-pedigree-function");
	itemsets.push_back(temp);
	temp.clear();
	process_continuous(age,temp,NUMBER_OF_INTERVALS,"age");
	itemsets.push_back(temp);
	temp.clear();
	itemsets.push_back(hasdm);
	h<<(NUMBER_OF_INTERVALS*8)+1<<" has-diabetes\n";
	h<<(NUMBER_OF_INTERVALS*8)+2<<" does-not-have-diabetes\n";
	g<<numdata<<" "<<(NUMBER_OF_INTERVALS*8)+2<<endl;
	for (int i = 0; i < numdata; ++i)
	{
		g<<numattributes<<" ";
		for (int j = 0; j < numattributes; ++j)
		{
			g<<itemsets[j][i]+(j*NUMBER_OF_INTERVALS)<<" ";
		}		
		g<<endl;
	}

	g.close();
	f.close();
}