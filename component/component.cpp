#include <list>
#include <vector>
#include <algorithm>
#include <stack>
#include <iostream>
#include <ostream>

using namespace std;

class Vertex;
typedef vector<Vertex> Vertices;

class Vertex
{
public:
	Vertex():pre(0), post(0), low(0)
	{
	}
	
	Vertex(const Vertex &other):pre(other.pre),post(other.post), low(other.low)
	{
		copy(other.adjList.begin(), other.adjList.end(), adjList.begin());
	}
	
	void insert(int vertex)
	{
		adjList.push_back(vertex);
	}
	
	int getPre(){return pre;}
	int getPost(){return post;}
	void setPre(int Pre){ pre = Pre; }
	void setPost(int Post){ post = Post; }
	Vertices::iterator next(Vertices &vertices)
	{
		list<int>::iterator next = adjList.begin();
		while(next != adjList.end()){
			if(!vertices[*next].getPre()) return vertices.begin()+*next;
			next++;
		}
		return vertices.end();
	}
	void print(ostream &o) const
	{
		o<<"pre: "<<pre<<", post: "<<post<<" List:";
		for(list<int>::const_iterator it=adjList.begin(); it!=adjList.end(); it++)
			o<<(*it)<<" ";
	}
	list<int>::const_iterator begin() const {return adjList.begin();}
	list<int>::const_iterator end() const {return adjList.end();}
	void eraseAll()
	{
		adjList.clear();
	}
	void erase(int vertex)
	{
		for(list<int>::iterator it = adjList.begin();it!=adjList.end();)
		{
			if(vertex == *it)
				it = adjList.erase(it);
			else
				it++;
		}
	}
	void updateLow(Vertices &vertices)
	{
		for(list<int>::iterator it = adjList.begin();it!=adjList.end();)
		{
			int pre = vertices[*it].getPre();
			if(pre == 0) continue;
			low = min(low, pre);
		}
	}
private:
	list< int > adjList;
	int pre, post, low;
};

ostream& operator<< (ostream& o, const Vertex& v)
{
	v.print(o);
	return o;
}

/*
1 3 0
2 1 0
3 8 4 0
4 6 0
5 4 0
6 7 0
7 5 0
8 1 0
*/
void setUp(Vertices &vertices)
{
	vertices.push_back(Vertex());
	vertices.push_back(Vertex());
	vertices.push_back(Vertex());
	vertices.push_back(Vertex());
	vertices.push_back(Vertex());
	vertices.push_back(Vertex());
	vertices.push_back(Vertex());
	vertices.push_back(Vertex());
	
	/*make adjList*/
	vertices[0].insert(3-1);
	vertices[1].insert(1-1);
	vertices[2].insert(8-1);
	vertices[2].insert(4-1);
	vertices[3].insert(6-1);
	vertices[4].insert(4-1);
	vertices[5].insert(7-1);
	vertices[6].insert(5-1);
	vertices[7].insert(1-1);
}

void digg(Vertices::iterator iter, Vertices &vertices, int &pre, stack<Vertices::iterator> &st)
{
	Vertices::iterator cur = iter;
	while(cur!=vertices.end()){
		cur->setPre(pre++);
		st.push(cur);
		cur = cur->next(vertices);
	}
}

/*
1 3 0
2 1 0
3 8 4 0
4 6 0
5 4 0
6 7 0
7 5 0
8 1 0
num pre post
1	1	14
2	15	16
3	2	13
4	5	12
5	8	9
6	6	11
7	7	10
8	3	4
*/
int DFS(Vertices &vertices)
{
	stack< Vertices::iterator > st;
	int index = 1;
	int rz;
	for(Vertices::iterator iter = vertices.begin(); 
				iter!=vertices.end(); iter++)
	{
		if(iter->getPre()) continue;
		digg(iter, vertices, index, st);
		while(st.size())
		{
			Vertices::iterator top = st.top();
			st.pop();
			Vertices::iterator next = top->next(vertices);
			digg(next, vertices, index, st);
			top->setPost(index++);
			rz = top-vertices.begin();
		}
	}
	return rz;
}

/*
1 3 0
2 1 0
3 8 4 0
4 6 0
5 4 0
6 7 0
7 5 0
8 1 0

1 2 8
2
3 1
4 3 5
5 7
6 4
7 6
8 3
*/
void reverse(Vertices &reversed, Vertices &vertices)
{
	for(Vertices::iterator iter = vertices.begin(); iter!=vertices.end(); iter++)
		reversed.push_back(Vertex());
	for(Vertices::iterator iter = vertices.begin(); iter!=vertices.end(); iter++)
	{
		for(list<int>::const_iterator it = iter->begin(); it!=iter->end(); it++)
			reversed[*it].insert(iter-vertices.begin());
	}
}

/*
1 3 0
2 1 0
3 8 4 0
4 6 0
5 4 0
6 7 0
7 5 0
8 1 0

1 3
2
3 8 4
4 6 
5 4
6 7
7 5
8 0
*/
void erase(int vertex, Vertices &vertices)
{
	vertices[vertex].eraseAll();
	for(Vertices::iterator iter = vertices.begin(); iter!=vertices.end(); iter++)
		iter->erase(vertex);
}

void print(Vertices &vs)
{
	for(Vertices::const_iterator iter = vs.begin(); iter!=vs.end(); iter++)
		cout<<iter-vs.begin()<<". "<<*iter<<endl;
}

int deleteSinkCompleteNode(int sink, Vertices &vertices)
{
	for(Vertices::iterator iter = vertices.begin(); 
				iter!=vertices.end(); iter++)
	{
		iter->setPre(0); iter->setPost(0);
	}
	vector<int> deleteNodes;
	stack< Vertices::iterator > st;
	int leader = sink;
	int index = 1;
	int rz;
	Vertices::iterator iter = vertices.begin()+sink;
	digg(iter, vertices, index, st);
	while(st.size())
	{
		Vertices::iterator top = st.top();
		st.pop();
		Vertices::iterator next = top->next(vertices);
		digg(next, vertices, index, st);
		top->setPost(index++);
		deleteNodes.push_back(top-vertices.begin());
		leader = max(leader, top-vertices.begin());
		rz = top-vertices.begin();
	}
	return rz;
}

int main()
{
	Vertices vertices;
	Vertices reversed;
	setUp(vertices);
	reverse(reversed, vertices);
	int source = DFS(reversed);
	int leader = deleteSinkCompleteNode(source, vertices);
	cout<<"sink node is "<<source<<endl;
	cout<<"leader of deleted nodes is "<<leader<<endl;
	erase(source, vertices);
	print(vertices);
	return 0;
}