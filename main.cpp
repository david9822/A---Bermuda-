//
//  main.cpp
//  bermuda
//
//  Created by David Svanholm on 2017-05-20.
//  Copyright © 2017 David Svanholm. All rights reserved.
//

#include <iostream>
#include <iomanip>

using namespace std;

int MovementCost(int n, int m){ //Beräknar avståndet mellan rutor. Antingen är det 14 (diagonalt) eller så är det 10. Används i pathfinder.
    int gCost;
    if (n!=0 && m!=0)
        gCost=14;
    else
        gCost=10;
    return gCost;
}

int HcostValue(int DestinationY, int DestinationX, int SquareY, int SquareX){ // En uppskattning av avståndet till slutmålet. Används i pathfinder
    int Distance;
    if  (DestinationX>SquareX)
        Distance=(DestinationX-SquareX)*10;
    else
        Distance=10*(SquareX-DestinationX);
    
    if  (DestinationY>SquareY)
        Distance=Distance +10*(DestinationY-SquareY);
    else
        Distance=Distance+10*(SquareY-DestinationY);
    
    return Distance;
}


void pathfinding(int StartY, int StartX, int DestinationY, int DestinationX){ //Hittar den kortaste vägen. Void eftersom den inte ska returnera några värden.
    
    class Square{// Klassen Square håller all information om rutorna
    public:
        int xPos;
        int yPos;
        int Gcost;
        int Hcost;
        int Fcost;
        int status;
        int parentX;
        int parentY;
    };
    
    Square list[48][57]; //48 rutor vertikalt och 57 rutor horisontellt
    
    for (int i=0; i<57; i++){ // Förser alla rutor med grundläggande info
        for (int j=0; j<48; j++){
            list[j][i].xPos=i;
            list[j][i].yPos=j;
            list[j][i].status=0;
            list[j][i].Gcost=0;
            list[j][i].Hcost=0;
            list[j][i].Fcost=0;
            list[j][i].parentY=-1;
            list[j][i].parentX=-1;
        }
    }
    
    /*
     status
     0=Unexplored
     1=Wall
     2=OpenList
     3=ClosedList
     4=Destination
     5=Path
     6=Start
     
     4,5,6 läggs till efter det att vägen hittats. I do-while loopen kan rutorna endast ha värde 0-3
     */
    int walls[]={0,0,0,1,0,2,0,3,0,4,0,5,0,6,0,7,0,8,0,9,0,10,0,11,0,12,0,13,0,14,0,15,0,16,0,17,0,18,0,19,0,20,0,21,0,22,0,23,0,24,0,25,0,26,0,27,0,28,0,29,0,30,0,31,0,32,0,33,0,34,0,35,0,36,0,37,0,38,0,39,0,40,0,41,0,42,0,43,0,44,0,45,0,46,0,47,0,48,0,49,0,50,0,51,1,0,1,5,1,23,1,28,1,38,1,45,1,51,2,0,2,5,2,15,2,23,2,28,2,38,2,45,2,51,3,0,3,5,3,15,3,28,3,38,3,45,3,51,4,0,4,5,4,15,4,23,4,24,4,25,4,26,4,27,4,28,4,38,4,45,4,51,5,0,5,5,5,15,5,28,5,38,5,45,5,51,6,0,6,5,7,6,7,7,7,8,7,9,7,10,7,11,7,12,7,13,7,14,7,15,6,23,6,28,6,38,6,45,6,51,7,0,7,5,6,15,7,23,7,28,7,38,7,45,7,51,8,0,8,5,8,15,8,23,8,28,8,38,8,45,8,51,9,0,9,5,9,15,9,23,9,28,9,38,9,45,9,51,10,0,10,5,10,15,10,23,10,28,10,38,10,45,10,51,11,0,11,5,11,15,11,23,11,28,11,38,11,45,11,51,12,0,12,5,12,15,12,23,12,28,12,38,12,45,12,51,13,0,13,5,13,15,13,23,13,28,13,38,13,45,13,51,14,0,14,1,14,2,14,3,14,5,14,6,14,7,14,8,14,9,14,10,14,11,14,12,14,13,14,14,14,23,14,24,14,25,14,26,14,27,14,28,14,29,14,30,14,32,14,33,14,34,14,35,14,36,14,37,14,38,14,39,14,40,14,42,14,43,14,44,14,45,14,51,14,52,14,53,14,54,14,55,14,56,15,0,15,51,15,56,16,0,16,1,16,2,16,3,16,4,16,5,16,51,16,56,17,0,17,43,17,44,17,45,17,51,17,56,18,0,18,6,18,7,18,9,18,28,18,29,18,30,18,31,18,32,18,33,18,34,18,35,18,36,18,37,18,38,18,39,18,40,18,42,18,43,18,44,18,45,18,46,18,51,18,56,19,0,19,7,19,9,19,20,19,21,19,22,19,23,19,24,19,25,19,28,19,40,19,41,19,42,19,46,19,56,20,0,20,1,20,2,20,3,20,4,20,5,20,6,20,7,20,8,20,9,20,10,20,11,20,12,20,13,20,14,20,15,20,16,20,20,20,21,20,22,20,23,20,24,20,25,20,28,20,46,20,56,21,0,21,16,21,28,21,46,21,51,21,56,22,0,22,16,22,28,22,46,22,51,22,56,23,0,23,16,23,28,23,46,23,51,23,56,24,0,24,16,24,28,24,46,24,51,24,56,25,28,26,28,27,28,28,28,29,28,30,28,31,28,32,28,33,28,34,28,35,28,36,28,37,28,38,28,39,28,40,28,41,28,42,28,43,28,44,28,45,28,46,28,47,28,25,0,26,0,27,0,28,0,29,0,30,0,31,0,32,0,33,0,34,0,35,0,36,0,37,0,38,0,39,0,40,0,41,0,42,0,43,0,44,0,45,0,46,0,47,0,25,51,26,51,27,51,28,51,29,51,30,51,31,51,32,51,33,51,34,51,35,51,36,51,37,51,38,51,39,51,40,51,41,51,42,51,43,51,44,51,45,51,46,51,47,51,25,46,26,46,27,46,28,46,29,46,30,46,31,46,32,46,33,46,34,46,35,46,36,46,37,46,38,46,39,46,40,46,41,46,42,46,43,46,44,46,45,46,46,46,47,46,47,1,47,2,47,3,47,4,47,5,47,6,47,7,47,8,47,9,47,10,47,11,47,12,47,13,47,14,47,15,47,16,47,17,47,18,47,19,47,20,47,21,47,22,47,23,47,24,47,25,47,26,47,27,47,29,47,30,47,31,47,32,47,33,47,33,47,34,47,35,47,36,47,37,47,38,47,39,47,40,47,41,47,42,47,43,47,44,47,45,47,47,47,48,47,49,47,50,21,52,21,55,25,53,25,54,25,55,25,56,26,56,27,56,28,56,29,56,30,56,31,56,32,56,33,56,34,56,35,56,36,56,37,56,38,56,39,56,40,56,41,56,41,55,41,54,41,53,41,52,35,50,35,47,25,16,26,16,27,16,28,16,29,16,30,16,31,16,32,16,33,16,34,16,35,16,37,16,37,15,37,14,37,13,37,12,38,12,39,12,40,12,41,12,42,12,43,12,44,12,45,12,46,12,47,12,40,13,40,14,40,15,40,16,40,17,40,19,40,21,40,22,40,23,40,24,40,25,40,26,40,27,41,19,42,19,43,19,44,19,45,19,46,19,36,12,35,12,34,12,33,12,32,12,31,12,30,12,29,12,28,12,27,12,26,12,25,12,24,12,23,12,22,12,21,12};
    
    int a=(sizeof(walls)/sizeof(*walls));
    for (int n=0;n<a; n=n+2){
        list[walls[n]][walls[n+1]].status=1;
    }
    cout << endl << endl << endl << endl;

    int CurrentSquare[2]={StartY, StartX}; //CurrentSquare är den ruta som undersöks i varje varv av den stora do-while-loopen.
    int DestinationSquare[2]={DestinationY, DestinationX}; //Då CurrentSquare[1]==DestinationY och CurrentSquare[0]==Destination
    
    int AdjacentSquare[]={0,-1,1,-1,1,0,1,1,0,1,-1,1,-1,0,-1,-1}; //Används i loopen för att det inte ska bli för mycket kod. Om du tar de två första värdena i listan kommer du en till vänster om CurrentSquare, nästa två positioner så kommer du snett uppåt tilll vänster om CurrentSquare osv.
    bool OpenListNotEmpty=true;
    
    list[CurrentSquare[0]][CurrentSquare[1]].status=3; //Startrutan läggs till i ClosedList
    do { // Ett varv motsvarar att en ruta läggs till i ClosedList och att vi får en ny CurrentSquare
        list[CurrentSquare[0]][CurrentSquare[1]].status=3; //Läggs till i Closed List
        
        for (int n=0; n<16; n=n+2){//Loop där de närliggande rutorna gås igenom. Varje varv kollar 1 ruta.Sammanlagt finns alltid 8 rutor runt varje ruta som undersöks. För rutorna längs kanten gäller inte det, men eftersom de alla är väggar kommer de inte att läggas till i OpenList och de kan heller därför inte läggas till i ClosedList.
            
            if (list[CurrentSquare[0]+AdjacentSquare[n]][CurrentSquare[1]+AdjacentSquare[n+1]].status==1){// Är det en vägg?
                continue;
            }
            if (list[CurrentSquare[0]+AdjacentSquare[n]][CurrentSquare[1]+AdjacentSquare[n+1]].status==3){//Är den i Closed List?
                continue;
            }
         
            if (list[CurrentSquare[0]+AdjacentSquare[n]][CurrentSquare[1]+AdjacentSquare[n+1]].status==2){// Är den i Openlist?{

                if (list[CurrentSquare[0]+AdjacentSquare[n]][CurrentSquare[1]+AdjacentSquare[n+1]].Gcost>list[CurrentSquare[0]][CurrentSquare[1]].Gcost+MovementCost(AdjacentSquare[n],AdjacentSquare[n+1])){ //Om det ett byte av parent ger ett lägre G-värde så ändras G-värde, F-värde och parent
                    list[CurrentSquare[0]+AdjacentSquare[n]][CurrentSquare[1]+AdjacentSquare[n+1]].Gcost=list[CurrentSquare[0]][CurrentSquare[1]].Gcost+MovementCost(AdjacentSquare[n], AdjacentSquare[n+1]); //Uppdaterar g-värde
                    list[CurrentSquare[0]+AdjacentSquare[n]][CurrentSquare[1]+AdjacentSquare[n+1]].Fcost=list[CurrentSquare[0]+AdjacentSquare[n]][CurrentSquare[1]+AdjacentSquare[n+1]].Gcost + list[CurrentSquare[0]+AdjacentSquare[n]][CurrentSquare[1]+AdjacentSquare[n+1]].Hcost; //Uppdaterar F-värdet
                    list[CurrentSquare[0]+AdjacentSquare[n]][CurrentSquare[1]+AdjacentSquare[n+1]].parentY=CurrentSquare[0]; //Ger rutan en ny parent
                    list[CurrentSquare[0]+AdjacentSquare[n]][CurrentSquare[1]+AdjacentSquare[n+1]].parentX=CurrentSquare[1];
                }
            }
            else { //Den är då unexplored. G,H och F-värde läggs till och den får parents.
                list[CurrentSquare[0]+AdjacentSquare[n]][CurrentSquare[1]+AdjacentSquare[n+1]].Hcost=HcostValue(DestinationSquare[0],DestinationSquare[1],(CurrentSquare[0]+AdjacentSquare[n]),(CurrentSquare[1]+AdjacentSquare[n+1]));
                
                list[CurrentSquare[0]+AdjacentSquare[n]][CurrentSquare[1]+AdjacentSquare[n+1]].Gcost=list[CurrentSquare[0]][CurrentSquare[1]].Gcost + MovementCost(AdjacentSquare[n],AdjacentSquare[n+1]);
                
                list[CurrentSquare[0]+AdjacentSquare[n]][CurrentSquare[1]+AdjacentSquare[n+1]].Fcost=list[CurrentSquare[0]+AdjacentSquare[n]][CurrentSquare[1]+AdjacentSquare[n+1]].Hcost+list[CurrentSquare[0]+AdjacentSquare[n]][CurrentSquare[1]+AdjacentSquare[n+1]].Gcost;
                
                list[CurrentSquare[0]+AdjacentSquare[n]][CurrentSquare[1]+AdjacentSquare[n+1]].parentY=CurrentSquare[0];
                
                list[CurrentSquare[0]+AdjacentSquare[n]][CurrentSquare[1]+AdjacentSquare[n+1]].parentX=CurrentSquare[1];
                
                
                list[CurrentSquare[0]+AdjacentSquare[n]][CurrentSquare[1]+AdjacentSquare[n+1]].status=2; //Lägger till i Openlist
            }
        }
        
        int b=0;
        for (int e=0; e<56; e++){//Tjänar två syften. Dels kollar den om det finns rutor kvar i openlist, dels bestämmer den storleken på LowestFcost. Storleken på den måste vara exakt, om det är för stort kommer det lägsta F-värdet alltid vara 0 och nästa ruta som undersöks att vara 0,0
            for (int j=0; j<48; j++){
                if (list[j][e].status==2){
                    b++;
                }
            }
        }
        
        if (b==0){
            OpenListNotEmpty=false;
            continue;
        }
        
        int LowestFcost[b*3]; //Gånger tre eftersom det ska finnas tre positioner för varje ruta
        int c=0;
        for (int i=0; i<57; i++){ //Upprättar en  lista med F-värden för de rutor som finns i OpenList samt kopplar det till X och Y kordinater
            for (int j=0; j<48; j++){
                if (list[j][i].status==2){
                    LowestFcost[c]=j;
                    LowestFcost[c+1]=i;
                    LowestFcost[c+2]=list[j][i].Fcost;
                    c=c+3;
                }
            }
        }
        
        int max=b*3-3;
        
        for(int i=0; i<max; i++){ //Sorterar F-värdena för de rutor som finns i OpenList
            int nrLeft=max-i;
            for (int j=0; j<nrLeft; j=j+3){ //Informationen om en ruta lagras på tre positioner i listan
                if (LowestFcost[j+2]>LowestFcost[j+5]){
                    int  temp=LowestFcost[j]; //Första positionen motsvarar y-värdet för rutan
                    LowestFcost[j]=LowestFcost[j+3];
                    LowestFcost[j+3]=temp;
                    
                    temp=LowestFcost[j+1]; //Andra positionen motsvarar x-värdet för rutan
                    LowestFcost[j+1]=LowestFcost[j+4];
                    LowestFcost[j+4]=temp;
                    
                    temp=LowestFcost[j+2]; //Tredje positionen motsvarar F-värdet för rutan
                    LowestFcost[j+2]=LowestFcost[j+5];
                    LowestFcost[j+5]=temp;
                }
            }
        }

        CurrentSquare[0]=LowestFcost[0]; //Den ruta med lägst F-kostnad blir den nya CurrentSquare
        CurrentSquare[1]=LowestFcost[1];

        if (CurrentSquare[0]==DestinationSquare[0] && CurrentSquare[1]==DestinationSquare[1])
            break; //Om nuvarande rutan är slutmålet så avslutas loopen
        
    } while (OpenListNotEmpty==true); //Då Openlist är tom och det inte finns fler rutor att undersöka så ska programmet avslutas. Då programmet byggdes lät jag programmet hitta vägen mellan två punkter som inte gick att nå. Programmet avslutades då
    
    bool elon=true; // Om en väg har hittats så ska vi visa en karta med vägen
    int parenty=DestinationY;
    int parentx=DestinationX;
    if (OpenListNotEmpty==true){
        do {
            int temp=parenty;
            parenty=list[parenty][parentx].parentY;
            parentx=list[temp][parentx].parentX;
            list[parenty][parentx].status=5;
            if (parenty==StartY && parentx==StartX)
                elon=false;
        } while(elon==true);
        
        list[DestinationY][DestinationX].status=4;
        list[StartY][StartX].status=6;
        
        for (int i=0; i<48; i++){// Visar hur kartan ser ut
            cout << endl;
            for (int j=0; j<57; j++){
                if (list[47-i][j].status==1) //För att kartan ska se rätt ut måste den sista vertikala skrivas ut först. Därav 47-i.
                    cout << "1";
                else if ((list[47-i][j].status==4) || (list[47-i][j].status==6))
                    cout << "x";
                else if (list[47-i][j].status==5)
                    cout << "o";
                else
                    cout << " ";
            }
        }
        
        string names[48][56]; // Listan är onödigt stor,men det var det minst komplicerade sättet jag kom på för att kunna skriva ut namnen på platserna med hjälp av de ursprungliga kordinaterna
        
        names[36][16]="LÄRARRUM";
        names[40][20]="71";
        names[40][18]="72";
        names[18][8]="TOALETT KOPIATORN";
        names[14][4]="53";
        names[13][14]="PROGRAMMERING";
        names[1][15]="51";
        names[3][23]="VAKTMÄSTARE";
        names[18][41]="TOA NARMAST UTGÅNG";
        names[14][31]="43";
        names[14][41]="44";
        names[5][23]="UPPEHALLSRUM";
        names[35][49]="1:a PROGRAMMERINGSSALEN";
        names[25][52]="BIBLIOTEKET";
        names[18][52]="RECEPTION";
    
        cout << endl << endl << "Avståndet mellan " << names[StartY][StartX] << " och " <<  names[DestinationY][DestinationX] << " är ca " <<0.1*list[DestinationY][DestinationX].Gcost << "m."; //En ruta är nästan exakt 1mx1m stor och eftersom avståndet görs 10ggr större i funktionen MovementCost så delar jag med 10 för att få ett värde för sträckan
    }
    else
        cout << "Ingen väg hittades";
    cout << endl << endl;
    }

int map(){ //Skriver ut kartan. Används i main.
    
    class Square{// Klassen Square håller all information om rutorna
    public:
        int xPos;
        int yPos;
        int Gcost;
        int Hcost;
        int Fcost;
        int status;
        int parentX;
        int parentY;
    };
    
    Square list[48][57];
    
    for (int i=0; i<57; i++){ // Förser alla rutor med grundläggande info
        for (int j=0; j<48; j++){
            list[j][i].xPos=i;
            list[j][i].yPos=j;
            list[j][i].status=0;
            list[j][i].Gcost=0;
            list[j][i].Hcost=0;
            list[j][i].Fcost=0;
            list[j][i].parentY=-1;
            list[j][i].parentX=-1;
        }
    }
    
    
    int walls[]={0,0,0,1,0,2,0,3,0,4,0,5,0,6,0,7,0,8,0,9,0,10,0,11,0,12,0,13,0,14,0,15,0,16,0,17,0,18,0,19,0,20,0,21,0,22,0,23,0,24,0,25,0,26,0,27,0,28,0,29,0,30,0,31,0,32,0,33,0,34,0,35,0,36,0,37,0,38,0,39,0,40,0,41,0,42,0,43,0,44,0,45,0,46,0,47,0,48,0,49,0,50,0,51,1,0,1,5,1,23,1,28,1,38,1,45,1,51,2,0,2,5,2,15,2,23,2,28,2,38,2,45,2,51,3,0,3,5,3,15,3,28,3,38,3,45,3,51,4,0,4,5,4,15,4,23,4,24,4,25,4,26,4,27,4,28,4,38,4,45,4,51,5,0,5,5,5,15,5,28,5,38,5,45,5,51,6,0,6,5,7,6,7,7,7,8,7,9,7,10,7,11,7,12,7,13,7,14,7,15,6,23,6,28,6,38,6,45,6,51,7,0,7,5,6,15,7,23,7,28,7,38,7,45,7,51,8,0,8,5,8,15,8,23,8,28,8,38,8,45,8,51,9,0,9,5,9,15,9,23,9,28,9,38,9,45,9,51,10,0,10,5,10,15,10,23,10,28,10,38,10,45,10,51,11,0,11,5,11,15,11,23,11,28,11,38,11,45,11,51,12,0,12,5,12,15,12,23,12,28,12,38,12,45,12,51,13,0,13,5,13,15,13,23,13,28,13,38,13,45,13,51,14,0,14,1,14,2,14,3,14,5,14,6,14,7,14,8,14,9,14,10,14,11,14,12,14,13,14,14,14,23,14,24,14,25,14,26,14,27,14,28,14,29,14,30,14,32,14,33,14,34,14,35,14,36,14,37,14,38,14,39,14,40,14,42,14,43,14,44,14,45,14,51,14,52,14,53,14,54,14,55,14,56,15,0,15,51,15,56,16,0,16,1,16,2,16,3,16,4,16,5,16,51,16,56,17,0,17,43,17,44,17,45,17,51,17,56,18,0,18,6,18,7,18,9,18,28,18,29,18,30,18,31,18,32,18,33,18,34,18,35,18,36,18,37,18,38,18,39,18,40,18,42,18,43,18,44,18,45,18,46,18,51,18,56,19,0,19,7,19,9,19,20,19,21,19,22,19,23,19,24,19,25,19,28,19,40,19,41,19,42,19,46,19,56,20,0,20,1,20,2,20,3,20,4,20,5,20,6,20,7,20,8,20,9,20,10,20,11,20,12,20,13,20,14,20,15,20,16,20,20,20,21,20,22,20,23,20,24,20,25,20,28,20,46,20,56,21,0,21,16,21,28,21,46,21,51,21,56,22,0,22,16,22,28,22,46,22,51,22,56,23,0,23,16,23,28,23,46,23,51,23,56,24,0,24,16,24,28,24,46,24,51,24,56,25,28,26,28,27,28,28,28,29,28,30,28,31,28,32,28,33,28,34,28,35,28,36,28,37,28,38,28,39,28,40,28,41,28,42,28,43,28,44,28,45,28,46,28,47,28,25,0,26,0,27,0,28,0,29,0,30,0,31,0,32,0,33,0,34,0,35,0,36,0,37,0,38,0,39,0,40,0,41,0,42,0,43,0,44,0,45,0,46,0,47,0,25,51,26,51,27,51,28,51,29,51,30,51,31,51,32,51,33,51,34,51,35,51,36,51,37,51,38,51,39,51,40,51,41,51,42,51,43,51,44,51,45,51,46,51,47,51,25,46,26,46,27,46,28,46,29,46,30,46,31,46,32,46,33,46,34,46,35,46,36,46,37,46,38,46,39,46,40,46,41,46,42,46,43,46,44,46,45,46,46,46,47,46,47,1,47,2,47,3,47,4,47,5,47,6,47,7,47,8,47,9,47,10,47,11,47,12,47,13,47,14,47,15,47,16,47,17,47,18,47,19,47,20,47,21,47,22,47,23,47,24,47,25,47,26,47,27,47,29,47,30,47,31,47,32,47,33,47,33,47,34,47,35,47,36,47,37,47,38,47,39,47,40,47,41,47,42,47,43,47,44,47,45,47,47,47,48,47,49,47,50,21,52,21,55,25,53,25,54,25,55,25,56,26,56,27,56,28,56,29,56,30,56,31,56,32,56,33,56,34,56,35,56,36,56,37,56,38,56,39,56,40,56,41,56,41,55,41,54,41,53,41,52,35,50,35,47,25,16,26,16,27,16,28,16,29,16,30,16,31,16,32,16,33,16,34,16,35,16,37,16,37,15,37,14,37,13,37,12,38,12,39,12,40,12,41,12,42,12,43,12,44,12,45,12,46,12,47,12,40,13,40,14,40,15,40,16,40,17,40,19,40,21,40,22,40,23,40,24,40,25,40,26,40,27,41,19,42,19,43,19,44,19,45,19,46,19,36,12,35,12,34,12,33,12,32,12,31,12,30,12,29,12,28,12,27,12,26,12,25,12,24,12,23,12,22,12,21,12};
    
    int a=(sizeof(walls)/sizeof(*walls));
    for (int n=0;n<a; n=n+2){
        list[walls[n]][walls[n+1]].status=1;
    }
    cout << endl;
    
    string names[]={"LÄRARRUM", "1", "71","2" ,"72", "3", "TOALETT KOPIATORN","4", "53", "5", "PROGRAMMERING", "6", "51","7",  "VAKTMÄSTARE","8",  "TOA NÄRMAST UTGÅNG" , "9", "43", "A", "44", "B" , "UPPEHÅLLSRUM" , "C", "1:A PROGRAMMERINGSSALEN", "D", "BIBLIOTEKET","E", "RECEPTION", "F"};
    
    int o=0;
    for (int i=0; i<48; i++){// Visar hur kartan ser
        if ((47-i)>40 && (i!=0)){
            cout << "             "  << names[o+1] << ". " << names[o];
            o=o+2;
        }
        else if ((47-i)>31 && (i!=0)){
            cout << "             "  << names[o+1] << ". " << names[o];
            o=o+2;
        }
        cout << endl;
        for (int j=0; j<57; j++){
            if ((47-i)==29 && j==14)
                cout << "1";
            
            else if ((47-i)==43 && j==23)
                cout << "2";
            
            else  if ((47-i)==43 && j==15)
                cout << "3";
            
            else if ((47-i)==18 && j==3)
                cout << "4";
            
            else if ((47-i)==7 && j==2)
                cout << "5";
            
            else if ((47-i)==10 && j==10)
                cout << "6";
            
            else if ((47-i)==3 && j==10)
                cout << "7";
            
            else if ((47-i)==2 && j==25)
                cout << "8";
            
            else if ((47-i)==18 && j==41)
                cout << "9";
            
            else if ((47-i)==7 && j==33)
                cout << "A";
            
            else if ((47-i)==7 && j==42)
                cout << "B";
            
            else if ((47-i)==8 && j==25)
                cout  <<"C";
            
            else if ((47-i)==41 && j==49)
                cout << "D";
            
            else if ((47-i)==33 && j==53)
                cout << "E";
            
            else if ((47-i)==18 && j==53)
                cout << "F";
            
            
            else if (list[47-i][j].status==1) 
                cout << "1" ;
            else
                cout << " ";
        }
    }
    return 0;
}

int main(){
    bool arc=true;
    char a;
    char b;
    char z,l; //Använder mig av char eftersom jag vill att användaren ska kunna skriva in vad som helst utan att programmet krashar.
    do{
    cout << map() << endl;
    cout << "        VÄLJ STARTPLATS: " << endl;
    cout << "        1. LÄRARRUM " << endl;
    cout << "        2. 71 " << endl;
    cout << "        3. 72 "<< endl;
    cout << "        4. TOALETT KOPIATORN "<< endl;
    cout << "        5. 53" << endl;
    cout << "        6. PROGRAMMERING "<< endl;
    cout << "        7. 51 "<< endl;
    cout << "        8. VAKTMÄSTARE "<< endl;
    cout << "        9. TOA NÄRMAST UTGÅNG "<< endl;
    cout << "        A. 43 " << endl;
    cout << "        B. 44 "<< endl;
    cout << "        C. UPPEHÅLLSRUM " << endl;
    cout << "        D. 1:A PROGRAMMERINGSSALEN "<< endl;
    cout << "        E. BIBLIOTEKET "<< endl;
    cout << "        F. RECEPTION "<< endl;
    cin >> a;
        if (a!='1' && a!='2' && a!='3' && a!='4' && a!='5' && a!='6' && a!='7' && a!='8' && a!='9' && a!='A' && a!='B' && a!='C' && a!='D' && a!='E' && a!='F' && a!='a' && a!='b' && a!='c' && a!='d' && a!='e' && a!='f'){
            cout << "Var inte så dum!" << " Tryck valfri tangent" ;
            cin >> z;
            continue;
        }
        
    cout << "        VÄLJ DESTINATION: " << endl;
    cout << "        1. LÄRARRUM " << endl;
    cout << "        2. 71 " << endl;
    cout << "        3. 72 "<< endl;
    cout << "        4. TOALETT KOPIATORN "<< endl;
    cout << "        6. PROGRAMMERING "<< endl;
    cout << "        7. 51 "<< endl;
    cout << "        8. VAKTMÄSTARE "<< endl;
    cout << "        9. TOA NÄRMAST UTGÅNG "<< endl;
    cout << "        A. 43 " << endl;
    cout << "        B. 44 " << endl;
    cout << "        C. UPPEHÅLLSRUM " << endl;
    cout << "        D. 1:A PROGRAMMERINGSSALEN "<< endl;
    cout << "        E. BIBLIOTEKET " << endl;
    cout << "        F. RECEPTION "<< endl;
    cout << "        G. AVSLUTA " << endl;
    cin >> b;
        if (b!='1' && b!='2' && b!='3' && b!='4' && b!='5' && b!='6' && b!='7' && b!='8' && b!='9' && b!='A' && b!='B' && b!='C' && b!='D' && b!='E' && b!='F' && b!='a' && b!='b' && b!='c' && b!='d' && b!='d' && b!='e' && b!='f' && b!='g'){
            cout << "Var inte så dum!" << endl <<  "Tryck valfri tangent ";
            cin >> l;
            continue;
        }
        
        if (b=='G' || b=='g'){
            arc=false;
            continue;
          }
        int ia=0; // Initierar en variabel nära användandet. Ges värdet noll eftersom jag annars får ett felmeddelande som säger att den riskerar att bli odefinierad när den ska användas längre ner i programmet. Eftersom vi redan kollat att  de införda värdena är giltiga så är det inga problem
        
        if ( a=='1' || a=='2' || a=='3' || a=='4' || a=='5' || a=='6'|| a=='7' || a=='8' || a=='9'){
            ia=(int)a; //Sparar värdet a som en int i variabeln ia. Gör det lättare när vi sedan ska anropa pathfinder-funktionen
            ia=ia-49; //Unicode för 1=49
        }
        else if ( a=='A' || a=='B' || a=='C' || a=='D' || a=='E' || a=='F'){
            ia=(int)a;
            ia=ia-56;
            }
        else {
            ia=(int)a;
            ia=ia-88;
        }
        int ib=0;

        if (b=='1' || b=='2' || b=='3' || b=='4' || b=='5' || b=='6'|| b=='7' || b=='8' || b=='9'){
            ib=(int)b;
            ib=ib-49;
        }
        
        else if ( b=='A' || b=='B' || b=='C' || b=='D' || b=='E' || b=='F'){
            ib=(int)b;
            ib=ib-56;
        }
        else {
            ib=(int)b;
            ib=ib-88;
        }
        
        string names[]={"LÄRARRUM", "71", "72", "TOALETT KOPIATORN", "53", "PROGRAMMERING", "51", "VAKTMÄSTARE", "TOA NÄRMAST UTGÅNG" ,"43", "44", "UPPEHÅLLSRUM" , "1:A PROGRAMMERINGSSALEN", "BIBLIOTEKET","RECEPTION"};

        if (ia==ib){
            cout << "Den kortaste vägen mellan " << names[ia] << " och " <<  names[ib] << " är att stå stilla. Bättre lycka nästa gång." << endl << "Tryck valfri tangent för att fortsätta" ;
            cin >> z;
            continue;
        }

    int smart[15]={36,40,40,18,14,13,1,3,18,14,14,5,35,25,18};
    int smart2[15]={16,20,18,8,4,14,15,23,41,31,41,23,49,52,52};
        
     pathfinding(smart[ia], smart2[ia], smart[ib], smart2[ib]); //This is were the magic happens.
        
        cout << "Vill du pröva igen?" << endl << "Tryck valfri tangent" << endl;
        char e;
        cin >> e;
        
    } while (arc==true);
    
        return 0;
}
