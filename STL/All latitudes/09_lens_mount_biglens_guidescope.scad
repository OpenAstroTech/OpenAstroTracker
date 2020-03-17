//$fn=10;

//lens_diameter = 62;

diam1 = 69.5; //[0:0.1:120]
diam2 = 65.5; //[0:0.1:120]
high1 = 6; //[0:0.1:30]


diam3 = 62.5; //[0:0.1:120]
diam4 = 62.5; //[0:0.1:120]
high2 = 25; //[0:0.1:30]

diam5 = 0; //[0:0.1:120]
diam6 = 0; //[0:0.1:120]
high3 = 0; //[0:0.1:30]

/* [Hidden] */
dmax = max(diam1,diam2,diam3,diam4,diam5,diam6);
gy = sqrt(max(dmax*dmax/4-41.9*41.9,20*20))+8;
gx_1 = sqrt((dmax/2+2.5)*(dmax/2+2.5)-gy*gy);
gx_2 = sqrt((dmax/2+2.5)*(dmax/2+2.5)-7.5*7.5);
gx_3 = max(sqrt((dmax/2+5)*(dmax/2+5)-7.5*7.5),57.1);

$fn=512;

difference (){
difference (){
difference (){
union (){
    cylinder (h=30,d=diam1+10);
    cube([91.3,15.5/2,30]);
    mirror([1,0,0]) cube([91.3,15.5/2,30]);
    mirror([0,1,0]) cube([91.3,15.5/2,30]);
    mirror ([1,0,0])mirror([0,1,0]) cube([91.3,15.5/2,30]);
    translate([0,0,6]) linear_extrude(height=18) polygon(points=[[-gx_1,-gy],[-55.1,-gy],[-57.1,-gy+2],[-gx_3,-7.49],[-gx_2,-7.49]]);
    mirror([1,0,0]) translate([0,0,6]) linear_extrude(height=18) polygon(points=[[-gx_1,-gy],[-55.1,-gy],[-57.1,-gy+2],[-gx_3,-7.49],[-gx_2,-7.49]]);
}
}
union () {
translate([0,0,-0.001]) cylinder(high1+0.1,d1=diam1,d2=diam2);
translate([0,0,high1-0.01]) cylinder(high2+0.1,d1=diam3,d2=diam4);
translate([0,0,high1+high2-0.02]) cylinder(high3+0.1,d1=diam5,d2=diam6);
}
}
cube([200,1,61],center=true);
translate ([91.3-15,5.9/2+4.45,15/2]) hex(5.9,5.9);
translate ([91.3-15,5.9/2+4.45,30-15/2]) hex(5.9,5.9);
translate ([-91.3+15,5.9/2+4.45,15/2]) hex(5.9,5.9);
translate ([-91.3+15,5.9/2+4.45,30-15/2]) hex(5.9,5.9);
translate([91.3-15,8,15/2]) rotate([90,0,0]) cylinder(h=20,d=3.4);
translate([91.3-15,8,30-15/2]) rotate([90,0,0]) cylinder(h=20,d=3.4);
translate([-91.3+15,8,15/2]) rotate([90,0,0]) cylinder(h=20,d=3.4);
translate([-91.3+15,8,30-15/2]) rotate([90,0,0]) cylinder(h=20,d=3.4);
translate([91.3-15,-3.95,15/2]) rotate([90,0,0])cylinder (h=5,d=5.8);
translate([91.3-15,-3.95,30-15/2]) rotate([90,0,0])cylinder (h=5,d=5.8);
translate([-91.3+15,-3.95,15/2]) rotate([90,0,0])cylinder (h=5,d=5.8);
translate([-91.3+15,-3.95,30-15/2]) rotate([90,0,0])cylinder (h=5,d=5.8);
translate([-47,-gy+3,15]) cube([10.2,10,8.2],center=true);
translate([47,-gy+3,15]) cube([10.2,10,8.2],center=true);
}


module hex(wid,height){
hull(){
cube([wid/1.7,wid,height],center = true);
rotate([0,120,0])cube([wid/1.7,wid,height],center = true);
rotate([0,240,0])cube([wid/1.7,wid,height],center = true);
}
}


