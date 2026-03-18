
#include <string>  
#include <iostream>



class Person{
    private:
    std::string first;
    std::string last;
    static int count;


    public:
    Person(std::string first, std::string last){
        this->first = first;
        this->last = last;
        count++;
    }

    // Person(std::string first, std::string last): first(first), last(last) {count++}
    Person(){count++;};



    void printName(){
        std::cout<<first<< " "<< last <<std::endl;
    }
    void setFirstName(std::string first){
        this->first = first;
    }

    void setLastName(std::string last){
        this->last = last;
    }

    std::string getFirstName(){
        return first;
    }
    std::string getLastName(){
        return last;
    }
    std::string getFullName(){
        return first + " "+ last;
    }

    void printCount(){
        std::cout<<count<<std::endl;
    }


};

class Employee : public  Person{

    private:
    std::string departement;


    public:
    Employee(std::string first, std::string last, std::string dep): Person(first, last), departement(dep){}
    Employee() = default;
    std::string getDepartement(){
        return this->departement;
    }

    static void setDepartement(std::string dep){
        this->departement = dep;
    }


};

int Person::count = 0;


int main(){
    

    Person p = Person("Henri", "Mertens");
    Person p2;


    p.setFirstName("Henri");
    p.setLastName("Mertens");

    p.printName();

    std::string name1 = p.getFullName();
    std::cout << name1<< std::endl;

    Employee e = Employee("Magnus", "Carlsen", "chess");
    e.setFirstName("Magnus");
    e.setLastName("Nakamura");
    e.printName();
    std::cout<< e.getDepartement()<< std::endl;


    // std::cout<<sizeof(e);
    Person p3("Magnus", "Carlsen");

    p.printCount();
    return 0;
}















