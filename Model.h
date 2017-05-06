/**
* @autor Camilo Barbosa Gracia
*
**/


#include<iostream>
#include<vector>
#include<fstream>
#include<sstream>
#include<stdlib.h>
#include<string.h>
#include <string>

void split(const std::string &s, char delim, std::vector<std::string> &elems)
{
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }
}

std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}



template <typename T>
std::string to_string(const T& value)
{
    std::ostringstream s;
    s << value;
    return s.str();
}



char * strTochar(std::string a)
{
    char * x=new char[a.length()+1];
    strcpy(x,a.c_str());
    return x;
}


int StrToInt(std::string x)
{
    return atoi(strTochar(x));
}

double StrToDouble(std::string x)
{
    return atof(strTochar(x));
}


struct VarModel
{
    int id;
    virtual std::string Columns()
    {
        return "";
    }
    virtual std::string File()
    {
        return "";
    }
    virtual std::string ModelName()
    {
        return "";
    }

    virtual std::string GetVar(std::string namevar)
    {
        if(namevar=="id")return to_string(id);
        return "";
    }

    virtual void SetVar(std::string namevar,std::string data)
    {
        if(namevar=="id")id=StrToInt(data);
    }

    virtual void Show()
    {


    }

    std::ostream &operator<<(std::ostream & out)
    {
        std::cout<<"";
        return out;
    }

};

template<template <class T> class m,class f1, class f2>
struct TowFK : VarModel
{
    std::string Columns(){return "fk1|fk2";}
    int fk1,fk2;
    virtual std::string GetVar(std::string namevar)
    {
        if(namevar=="fk1")return to_string(fk1);
        if(namevar=="fk2")return to_string(fk2);
        return "";
    }

    virtual void SetVar(std::string namevar,std::string data)
    {
        if(namevar=="fk1")fk1=StrToInt(data);
        if(namevar=="fk1")fk2=StrToInt(data);
    }

    f1* GetItem1()
    {
        m<f1> m1;
        if(m1.DataSize()>0)
            return &m1.data[fk1];
        return NULL;
    }

    f2* GetItem2()
    {
        m<f2> m2;
        if(m2.DataSize()>0)
            return &m2.data[fk2];
        return NULL;
    }
};






template<class T>
class model
{

public:
    struct collect
    {
        std::vector<int> id;
        std::vector<T*> data;
        void push_back(int _id,T* item)
        {
            id.push_back(_id);
            data.push_back(item);
        }
        void clear()
        {
            id.clear();
            data.clear();
        }

    };
    ~model() {}

    std::vector< T > data;
    struct collect Selected;
    int DataSize()
    {
        if(data.size()==0)Read();
        return (int)data.size();
    }

    int SelectionSize()
    {
        return (int)Selected.data.size();
    }

    model(char delimiter_col='\t', char delimiter_row='\n')
    {
        T item;
        name=item.ModelName();
        columns=split(item.Columns(),'|');

        file=strTochar(item.File());
        delimiterCol=delimiter_col;
        delimiterRow=delimiter_row;

    }
    model(std::string modelName, std::string columns, std::string dirWithExt="/|.txt", char delimiter_col='\t',  char delimiter_row='\n')
    {
        name=modelName;
        columns=split(columns,'|');
        file=strTochar(split(dirWithExt,'|')[0]+modelName+split(dirWithExt,'|')[1]);
        delimiterCol=delimiter_col;
        delimiterRow=delimiter_row;

    }

    bool Create(T& item)
    {
        Read();
        creating=true;
        if(Validator(item))
        {
            data.push_back(item);
            Write();
            creating=false;
            return true;
        }
        creating=false;
        return false;

    }


    bool Where(std::string _qry)
    {
        Read();
        Selected.clear();
        if(!data.size())return false;
        std::vector<std::string> qry=split(_qry,'|');//AND
        bool tr=false;
        for(int i=0; i<(int)data.size(); i++)
        {
            if(Qry(qry,data[i])||_qry=="All")
            {
                tr=true;
                Selected.push_back(i,&data[i]);
            }
        }
        return tr;
    }

    void Show()
    {
        Where("All");
        for(int i=0; i<SelectionSize(); i++)
        {
            Selected.data[i]->Show();
        }
    }

    void Update(std::string _qry)
    {
        for(int j=0; j<SelectionSize(); j++)
        {
            std::vector<std::string> qry=split(_qry,'|');
            T item;
            CopyElement(item,Selected.data[j]);
            for(int i=0; i<(int)qry.size(); i++)
            {
                std::vector<std::string> varqry=split(qry[i],'=');
                item.SetVar(varqry[0],varqry[1]);
            }
            if(Validator(item,true))
            {
                CopyElement(Selected.data[j],item);
            }

        }
        Write();

    }
    bool Delete()
    {
        if(!SelectionSize())return false;
        for(int i=0; i<SelectionSize();)
        {
            Selected.data.erase(Selected.data.begin()+i);
            data.erase(data.begin()+(Selected.id[i]));
            Selected.id.erase(Selected.id.begin()+i);
        }
        Write();
        return true;
    }
    template<class t>
    void MultiRelationalShip_Create(model<t>& other,std::string resent_qry,std::string forain_qry,std::string dirWithExt="/|.txt")
    {


        model<TowFK<model,T,t> > x(name+"-"+other.name,"fk1|fk2",dirWithExt);
        if(Where(resent_qry)&&other.Where(forain_qry))
        {
            for(int i=0; i<SelectionSize(); i++)
            {
                for(int j=0; j<other.SelectionSize(); j++)
                {
                    struct TowFK<model,T,t> item;
                    item.fk1=Selected.id[i];
                    item.fk2=other.Selected.id[j];
                    x.Create(item);
                }
            }
        }

    }
    std::string name;

    bool FindAllUniqueByColumn(std::string column)
    {
        Selected.clear();
        for(int i=0;i<DataSize();i++)
        {
            bool tr=true;
            for(int j=0;j<(int)Selected.data.size();j++)
            {
                if(data[i].GetVar(column)==Selected.data[j]->GetVar(column))
                    {
                        tr=false;
                        break;
                    }
            }
           if(tr)
           {
               Selected.data.push_back(&data[i]);
               Selected.id.push_back(i);
           }
        }
        if(Selected.data.size()>0)return true;
        return false;

    }


private:

    bool creating;
    char delimiterCol, delimiterRow;
    char* file;
    std::vector<std::string> columns;

    void CopyElement(T& one, T* d)
    {
        for(int i=0; i<(int)columns.size(); i++)
        {
            std::string col=split(columns[i],':')[0];
            one.SetVar(col,d->GetVar(col));
        }
    }

    void CopyElement(T* one, T& d)
    {
        for(int i=0; i<(int)columns.size(); i++)
        {
            std::string col=split(columns[i],':')[0];
            one->SetVar(col,d.GetVar(col));
        }
    }

    bool Qry(std::vector<std::string>& qry, T & item)
    {
        for(int i=0; i<(int)qry.size(); i++)
        {
            std::vector<std::string> varqry=split(qry[i],'=');
            std::string varresult=item.GetVar(varqry[0]);
            if(varresult==""||varresult!=varqry[1])
            {
                return false;
            }
        }
        return true;
    }

    bool Validator(T& item,bool resent=false)
    {
        bool tr=true;
        for(int i=0; i<(int)columns.size(); i++)
        {

            std::vector<std::string> qry=split(columns[i],':');
            if(qry.size()>1)
            {
                if(qry[1]=="unique"&&!resent)
                {
                    if(item.GetVar(qry[0])=="")return false;
                    if(Where(qry[0]+"="+item.GetVar(qry[0])))
                        tr=false;
                }
                if(qry[1]=="autoincrement"&&!resent&&creating)
                {
                    if(Where("All"))
                    {
                        int id=StrToInt(Selected.data[DataSize()-1]->GetVar(qry[0]));
                        id++;
                        item.SetVar(qry[0],to_string(id));

                    }else
                    item.SetVar(qry[0],"0");
                }
            }
        }
        return tr;
    }

    void Read()
    {
        std::ifstream open(file,std::ios::in);
        data.clear();
        if(open.is_open())
        {
            std::string x;
            while(getline(open,x))
            {
                T item;
                std::vector<std::string> result=split(x,delimiterCol);
                if(result.size()==columns.size())
                {
                    for(int j=0;j<(int)columns.size();j++)
                    {
                        std::vector<std::string> col=split(columns[j],':');
                        item.SetVar(col[0],result[j]);
                    }
                    data.push_back(item);
                }

            }

        }
        open.close();
    }

    void Write()
    {
        std::ofstream open(file,std::ios::out);
        if(open.is_open())
        {

            for(int i=0; !open.eof()&&i<(int)data.size(); i++)
            {

                std::vector<std::string> result;
                for(int j=0;j<(int)columns.size();j++)
                {
                    std::vector<std::string> x=split(columns[j],':');
                    result.push_back(data[i].GetVar(x[0]));
                }
                for(int j=0; j<(int)result.size()-1; j++)
                {
                    open<<result[j]<<delimiterCol;
                }
                open<<result[result.size()-1]<<delimiterRow;
            }

        }
        open.close();
    }

};


