#include<bits/stdc++.h>
using namespace std;
#define SUPPORT 0.4

fstream f("transactions.txt",ios::in);
fstream o("tree.txt", ios::out);
vector<vector<int> > transactions;
vector<vector<int> > frequent_itemsets;
map<vector<int>, int> support_count;
vector<int> one_frequent_itemsets;
map<int,int> one_frequent_support_count;
int num_trans;
int num_items;

struct node
{
	int item;
	int count;
	node *parent;
	map<int, node* > children;
	set<int> children_vals;
};

map<int, vector<node*> > pointers;
node *root;

bool comp_support(int a,int b)
{
	return one_frequent_support_count[a]>one_frequent_support_count[b];
}

bool comp_support_rev(int a,int b)
{
	return one_frequent_support_count[a]<one_frequent_support_count[b];
}

void find_one_frequent_itemsets()
{
	vector<int> temp_support_count(num_items+1,0);
	for (int i = 0; i < num_trans; ++i)
	{
		int num_items_in_trans;
		f>>num_items_in_trans;
		vector<int> temp_trans;
		for(int j=0; j<num_items_in_trans; j++)
		{
			int t;
			f>>t;
			temp_trans.push_back(t);
			temp_support_count[t]++;
		}
		transactions.push_back(temp_trans);
	}
	for (int i = 1; i <= num_items; ++i)
	{
		if(((double)temp_support_count[i]/(double)num_trans) >= SUPPORT)
		{
			one_frequent_itemsets.push_back(i);
			one_frequent_support_count[i] = temp_support_count[i];
		}
	}
	for (int i = 0; i < transactions.size(); ++i)
	{
		sort(transactions[i].begin(), transactions[i].end(), comp_support);
		int flag = 0;
		for(int j=transactions[i].size()-1;j>=0;j--)
		{
			//if item in transaction does not have enough support, remove it
			if(((double)temp_support_count[transactions[i][j]]/(double)num_trans) >= SUPPORT) 
			{
				flag = 1;
				if(j!=transactions[i].size())
					transactions[i].erase(transactions[i].begin()+j+1, transactions[i].end());
				break;
			}
		}
		if(!flag) //removing transactions which don't have any frequent 1-items
		{
			transactions.erase(transactions.begin()+i);
			i--;
		}
	}
	// printing processed transactions
	// for (int i = 0; i < transactions.size(); ++i)
	// {
	// 	cout<<i+1<<" ";
	// 	for(int j=0;j<transactions[i].size();j++)
	// 		cout<<transactions[i][j]<<" ";
	// 	cout<<endl;
	// }
}

void add_transaction(node *root, vector<int> transaction, int ind)
{
	if(ind == transaction.size())
		return;
	if(root->children_vals.find(transaction[ind]) == root->children_vals.end())
	{
		root->children[transaction[ind]] = new node;
		root->children[transaction[ind]]->item = transaction[ind];
		root->children[transaction[ind]]->count = 1;
		root->children[transaction[ind]]->parent = root;
		root->children_vals.insert(transaction[ind]);
		pointers[transaction[ind]].push_back(root->children[transaction[ind]]);
	}
	else
		root->children[transaction[ind]]->count++;
	add_transaction(root->children[transaction[ind]], transaction, ind+1);
}

void build_fptree()
{
	root = new node;
	root->item = 0;
	for(int i = 0;i<transactions.size();i++)
		add_transaction(root, transactions[i], 0);
}

void print_tree(node *root)
{
	o<<root->item<<" ";
	for(set<int>::iterator it = root->children_vals.begin(); it!=root->children_vals.end(); ++it)
	{
		o<<*it<<" "<<root->children[*it]->count<<" ";
	}
	o<<endl;
	for(set<int>::iterator it = root->children_vals.begin(); it!=root->children_vals.end(); ++it)
	{
		print_tree(root->children[*it]);
	}
}

void project(node *root, int item, map<int, vector<node*> > ptrs, node *&new_root, map<int, vector<node*> > &new_ptrs)
{
	vector<node*> item_ptrs = ptrs[item];
	vector<node*> parent_nodes(item_ptrs.size());
	new_root = new node;
	new_root->item = 0;
	new_root->count = 0;
	map<node*,node*> created_nodes;
	created_nodes[root] = new_root;
	
	for (int i = 0; i < item_ptrs.size(); ++i)
	{
		if(item_ptrs[i]->parent->item!=0)
		{
			parent_nodes[i] = new node;
			parent_nodes[i]->item = item_ptrs[i]->parent->item;
			parent_nodes[i]->count = item_ptrs[i]->count;
			created_nodes[item_ptrs[i]->parent] = parent_nodes[i];
			new_ptrs[parent_nodes[i]->item].push_back(parent_nodes[i]);
		}
		else
		{
			new_root->count = item_ptrs[i]->count;
			parent_nodes[i] = new_root;
		}
	}
	for (int i = 0; i < parent_nodes.size(); ++i)
	{
		node *temp = new_root;
		if(item_ptrs[i]->parent->item!=0)
			temp = item_ptrs[i]->parent->parent;
		node *cur = parent_nodes[i];
		int inc = item_ptrs[i]->count;
		while(cur->item!=0)
		{
			if(created_nodes.count(temp)==0)
			{
				cur->parent = new node;
				cur->parent->item = temp->item;
				cur->parent->count = inc;
				cur->parent->children[cur->item] = cur;
				cur->parent->children_vals.insert(cur->item);
				created_nodes[temp] = cur->parent;
				new_ptrs[cur->parent->item].push_back(cur->parent);
			}
			else
			{				
				node *it = created_nodes[temp];
				it->count += inc;
				cur->parent = it;
				it->children[cur->item] = cur;
				it->children_vals.insert(cur->item);		
			}
			if(temp->item!=0)
				temp = temp->parent;
			cur = cur->parent;
		}
	}
	
}

void find_frequent_itemsets_ending_with(int ind, node *root, vector<int> cur_itemset, map<int, vector<node*> > cur_ptrs)
{
	if(ind<0)
		return;
	int cnt = 0;
	vector<node*> temp = cur_ptrs[one_frequent_itemsets[ind]];
	for (int i = 0; i < temp.size(); ++i)
	{
		cnt += temp[i]->count;
	}
	double support_for_itemset = (double)cnt/num_trans;
	if(support_for_itemset >= SUPPORT)
	{
		cur_itemset.insert(cur_itemset.begin(),one_frequent_itemsets[ind]);
		frequent_itemsets.push_back(cur_itemset);
		support_count[cur_itemset] = cnt;
		node *new_root;
		map<int, vector<node*> > new_ptrs;
		project(root, one_frequent_itemsets[ind], cur_ptrs, new_root, new_ptrs);
		for(int i=ind-1;i>=0;i--)
		{
			find_frequent_itemsets_ending_with(i,new_root,cur_itemset,new_ptrs);
		}		
	}
}

void generate_frequent_temsets()
{
	sort(one_frequent_itemsets.begin(),one_frequent_itemsets.end(),comp_support);
	for (int i = one_frequent_itemsets.size()-1; i >=0 ; --i)
	{
		vector<int> temp;
		find_frequent_itemsets_ending_with(i,root,temp,pointers);
	}
	
}

void write_frequent_itemsets()
{
	fstream g("frequent_itemsets.txt",ios::out);
	g<<num_trans<<" "<<num_items<<endl;
	g<<frequent_itemsets.size()<<endl;
	for (int i = 0; i < frequent_itemsets.size(); ++i)
	{
		g<<frequent_itemsets[i].size()<<" ";
		g<<support_count[frequent_itemsets[i]]<<" ";
		sort(frequent_itemsets[i].begin(), frequent_itemsets[i].end());
		for(int j=0;j<frequent_itemsets[i].size();++j)
			g<<frequent_itemsets[i][j]<<" ";		
		g<<endl;
	}
}

bool fre(vector<int> a,vector<int> b)
{
	sort(a.begin(), a.end());
	sort(b.begin(),b.end());
	if(a.size()<b.size())
		return true;
	if(a.size()>b.size())
		return false;
	for(int i = 0;i<a.size();i++)
	{
		if(a[i]<b[i])
			return true;
		if(a[i]>b[i])
			return false;
	}
	return false;
}

int main()
{
	f>>num_trans;
	f>>num_items;
	find_one_frequent_itemsets();
	build_fptree();
	print_tree(root);
	generate_frequent_temsets();
	sort(frequent_itemsets.begin(),frequent_itemsets.end(), fre);
	write_frequent_itemsets();
	return 0;
}