speakerOuter = 31.5;
speakerInner = 26.5;
speakerTickness = 2.2;
tickness = 3.2;


module speakerHole() {
	$fn = 64;
	innerHeight = tickness - speakerTickness;

	cylinder(h=speakerTickness, r=speakerOuter/2, center=true);
	translate([0, 0, -(speakerTickness/2 + (tickness - speakerTickness) / 2)])
		cylinder(h=innerHeight+2, r=speakerInner/2, center=true);
}

module screwHoler() {
	$fn = 16;
	difference() {
		cube(size=[(speakerOuter+6) * 2, tickness, 10], center=true);
		translate([0, 0, 1.6]) rotate([90, 0, 0]) cylinder(r=1.6, h=tickness+1, center=true);
		translate([15, 0, 1.6]) rotate([90, 0, 0]) cylinder(r=1.6, h=tickness+1, center=true);
		translate([30, 0, 1.6]) rotate([90, 0, 0]) cylinder(r=1.6, h=tickness+1, center=true);
		translate([-15, 0, 1.6]) rotate([90, 0, 0]) cylinder(r=1.6, h=tickness+1, center=true);
		translate([-30, 0, 1.6]) rotate([90, 0, 0]) cylinder(r=1.6, h=tickness+1, center=true);
	}
}


difference() {
	union() {
		cube(size=[(speakerOuter+6) * 2, speakerOuter+6, tickness], center=true);
		translate([0, (speakerOuter+6)/2 + tickness/2, 10/2 - tickness/2]) 
		screwHoler();
	}
	union() {
		translate([-speakerOuter/2-2, 0, 1])
			speakerHole();
		translate([speakerOuter/2+2, 0, 1])
			speakerHole();
	}
}