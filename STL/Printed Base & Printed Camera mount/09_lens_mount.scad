//$fn=10;

//lens_diameter = 62;

diam1 = 69.5;
diam2 = 69.5;
high1 = 6;


diam3 = 62.5;
diam4 = 62.5;
high2 = 25;

high3 = 0;
diam5 = 0;
diam6 = 0;

length = 30; //reduce this if needed

/*union() {
translate([0,0,20]) cube([(diam1/2)+5,100,10]);
mirror([1,0,0]) translate([0,0,20]) cube([(diam1/2)+5,100,10]);
mirror([0,1,0]) translate([0,0,20]) cube([(diam1/2)+5,100,10]);
mirror([1,0,0])mirror([0,1,0]) translate([0,0,20]) cube([(diam1/2)+5,100,10]);
}*/

difference (){
difference (){
difference (){
union (){
    cylinder (h=30,d=diam1+10,$fn=512);
    cube([67,15.5/2,30]);
    mirror([1,0,0]) cube([67,15.5/2,30]);
    mirror([0,1,0]) cube([67,15.5/2,30]);
    mirror ([1,0,0])mirror([0,1,0]) cube([67,15.5/2,30]);
}
}
union () {
translate([0,0,-0.001]) cylinder(high1+0.1,d1=diam1,d2=diam2,$fn=512);
translate([0,0,high1-0.01]) cylinder(high2+0.1,d1=diam3,d2=diam4,$fn=512);
translate([0,0,high1+high2-0.02]) cylinder(high3+0.1,d1=diam5,d2=diam6,$fn=512);
    
union() {
translate([0,0,length+0.001]) cube([(diam1/2)+4.2,100,100]);
mirror([1,0,0]) translate([0,0,length+0.001]) cube([(diam1/2)+4.2,100,100]);
mirror([0,1,0]) translate([0,0,length+0.001]) cube([(diam1/2)+4.2,100,100]);
mirror([1,0,0])mirror([0,1,0]) translate([0,0,length+0.001]) cube([(diam1/2)+4.2,100,100]);
}
    
}
}
cube([150,1,61],center=true);
translate ([67-15,5.9/2+4.45,15/2]) hex(5.9,5.9);
translate ([67-15,5.9/2+4.45,30-15/2]) hex(5.9,5.9);
translate ([-67+15,5.9/2+4.45,15/2]) hex(5.9,5.9);
translate ([-67+15,5.9/2+4.45,30-15/2]) hex(5.9,5.9);
translate([67-15,8,15/2]) rotate([90,0,0]) cylinder(h=20,d=3.4,$fn=512);
translate([67-15,8,30-15/2]) rotate([90,0,0]) cylinder(h=20,d=3.4,$fn=512);
translate([-67+15,8,15/2]) rotate([90,0,0]) cylinder(h=20,d=3.4,$fn=512);
translate([-67+15,8,30-15/2]) rotate([90,0,0]) cylinder(h=20,d=3.4,$fn=512);
translate([67-15,-3.95,15/2]) rotate([90,0,0])cylinder (h=5,d=5.8,$fn=512);
translate([67-15,-3.95,30-15/2]) rotate([90,0,0])cylinder (h=5,d=5.8,$fn=512);
translate([-67+15,-3.95,15/2]) rotate([90,0,0])cylinder (h=5,d=5.8,$fn=512);
translate([-67+15,-3.95,30-15/2]) rotate([90,0,0])cylinder (h=5,d=5.8,$fn=512);
}


module hex(wid,height){
hull(){
cube([wid/1.7,wid,height],center = true);
rotate([0,120,0])cube([wid/1.7,wid,height],center = true);
rotate([0,240,0])cube([wid/1.7,wid,height],center = true);
}
}


