module base()
{translate([0,0,1.5])
{
import("C:/Users/a.airaghi/Desktop/__disegniParti/Motor_mount/files/motorMount.stl");}}
module par1()
    {difference()
        {
        union()
            {
            translate([0,-23.5,0])
    color("red")cube([3,43,33]);
                translate([-5,-23.5,0])        
cube([5,2,33]);}
    translate([-7,-2,10.5])color("red")cube([16,15,7]);
            translate([-10,-13.5,11])color("red")cube([16,4,6]);
    translate([-5,-5,14])
    rotate([0,90,0])
    color("red")
    cylinder(r=2,h=10);
    translate([-5,16,14])
    rotate([0,90,0])
    color("red")
    cylinder(r=2,h=10);
    translate([-4,3,6.5])
cube([20,6,15]);
        }
    }
        
module par2()
    {difference()
        {
        translate([-39,-2,0])
    color("blue")cube([40,2,33]);

    translate([-7,-2.5,10.5])color("blue")cube([16,15,7]);

    hull()
    {
    translate([-10,5,14])
    rotate([90,0,0])
    color("blue")cylinder(r=4.5,h=10);
    translate([-22,5,14])
    rotate([90,0,0])
    color("blue")cylinder(r=4.5,h=10);}
               
    translate([-33.5,3,5])
        rotate([90,0,0])        
    cylinder(r=2,h=10);
                translate([-33.5,3,23])
        rotate([90,0,0])        
    cylinder(r=2,h=10);    
        }
        

    
        
    }

module par3()
    {difference()
        {
        translate([-40,-23.5,31])
    color("green")cube([41.5,43,2]);
translate([-4,14.2,25])
 color("green")cylinder(r=2,h=10)  ; 
    translate([-4,-15.4,25])
 color("green")cylinder(r=2,h=10)  ; 
     translate([-34,-15.4,25])
 color("green")cylinder(r=2,h=10)  ; 
    translate([-34,5,25])
 color("green")cylinder(r=2,h=10)  ; 
    translate([-15,5,25])
 color("green")cylinder(r=2,h=10)  ; 
 translate([-40,10,31])
    color("green")cube([30,11,2]); 
   
           
        }
    }

module par4()
    {difference()
        {
        translate([-40,-23.5,0])
    color("orange")cube([41.5,43,2]);
translate([-40,-23.5,0])
    color("orange")cube([35,16.5,2]);
 translate([-40,0,0])
    color("orange")cube([35,23,2]);  
           
        }
    }

//base();
rotate([0,90,0])
union()
    {
par1();
par2();
par3();
par4(); 
        }
//par1();


    

        
        
        
       

   
    

