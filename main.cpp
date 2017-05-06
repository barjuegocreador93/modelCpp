

#include"Model.h"

using namespace std;

/**Una tienda necesita registrar sus productos en un archivo, verlos y edintificarlo
y calcular la gancias totales que debe tener su bodega**/

struct Producto : VarModel
{
    string ModelName()
    {
        return "Producto";
    }
    string File()
    {
        return "producto.txt";
    }
    string Columns()
    {
        return "id:autoincrement|nombre:unique|cantidad|cost";
    }

    string name;
    int cant;
    float cost;


    void SetVar(string var, string data)
    {
        if(var=="nombre")name=data;
        if(var=="cantidad")cant=StrToInt(data);
        if(var=="cost")cost=StrToDouble(data);

        return VarModel::SetVar(var,data);
    }

    string GetVar(string var)
    {

        if(var=="nombre")return name;
        if(var=="cantidad")return to_string(cant);
        if(var=="cost")return to_string(cost);

        return VarModel::GetVar(var);
    }

    void Show()
    {
        cout<<"\n----------\n";
        cout<<"id: "<<id<<endl;
        cout<<"name: "<<name<<endl;

    }


};

model<Producto> bodega;
void CrearProducto();
void BuscarPorId();
void BuscarPorNombre();
void Coste();
int main()
{
    char d;
    while(d!='6')
    {
        cout<<"1. Crear producto"<<endl;
        cout<<"2. Buscar por id"<<endl;
        cout<<"3. Lista de productos"<<endl;
        cout<<"4. Buscar por nombre"<<endl;

        cout<<"5. Coste total en bodega"<<endl;
        cout<<"6.Salir"<<endl;
        cin>>d;
        switch(d)
        {
            case '1':CrearProducto();break;
            case '2':BuscarPorId();break;
            case '3':bodega.Show();break;
            case '4':BuscarPorNombre();break;
            case '5':Coste();break;

        }
    }


    return 0;
}

void CrearProducto()
{

    Producto nuevo;
    while(!bodega.Create(nuevo))
    {
        cout<<"Nomrbe del producto: ";
        cin>>nuevo.name;

        cout<<"Canitidad del producto: ";
        cin>>nuevo.cant;

        cout<<"Presio del producto: ";
        cin>>nuevo.cost;
    }
}

void BuscarPorId()
{
    string id;
    cout<<"Escriba un id: ";
    cin>>id;

    if(bodega.Where("id="+id))
    {
        cout<<"Existe y es: "<<bodega.Selected.data[0]->name<<endl;
    }else
    {
        cout<<"El id no existe"<<endl;
    }

}

void BuscarPorNombre()
{
    string id;
    cout<<"Escriba un nombre: ";
    cin>>id;

    if(bodega.Where("nombre="+id))
    {
        cout<<"Existe y es: "<<bodega.Selected.data[0]->name<<endl;
    }else
    {
        cout<<"El nombre no existe"<<endl;
    }

}


void Coste()
{
    double coste=0.0;

    bodega.Where("All");
    for(int i=0;i<bodega.Selected.data.size();i++)
    {
        Producto * aux= bodega.Selected.data[i];
        coste+= aux->cant * aux->cost;
    }

    cout<<"el coste es: "<<coste<<endl;
}
