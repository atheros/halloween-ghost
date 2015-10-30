SERVO_WIDTH = 23.1;
SERVO_HEIGHT = 12.5;
SERVO_SCREW = 2.2;
SERVO_SCREW_DISTANCE = 27.1;
MOUNT_WIDTH = 37;
MOUNT_HEIGHT = 28;
MOUNT_TICKNESS = 2.5;
MOUNT_SCREW = 4;


module servo() {
	$fn = 32;
	cube(size=[SERVO_WIDTH, SERVO_HEIGHT, MOUNT_TICKNESS + 0.1], center=true);
	translate([SERVO_SCREW_DISTANCE/2, 0, 0])
		cylinder(d=SERVO_SCREW, h=MOUNT_TICKNESS + 0.1, center=true);
	translate([-SERVO_SCREW_DISTANCE/2, 0, 0])
		cylinder(d=SERVO_SCREW, h=MOUNT_TICKNESS + 0.1, center=true);
}

module mountScrews() {
	$fn = 32;
	dist_x = MOUNT_WIDTH/2 - MOUNT_SCREW*1.2;
	dist_y = MOUNT_HEIGHT - MOUNT_SCREW*1.2;
	translate([-dist_x, -dist_y, 0])
		cylinder(d=MOUNT_SCREW, h=MOUNT_TICKNESS + 0.1, center=true);
	translate([dist_x, -dist_y, 0])
		cylinder(d=MOUNT_SCREW, h=MOUNT_TICKNESS + 0.1, center=true);
	translate([0, -dist_y, 0])
		cylinder(d=MOUNT_SCREW, h=MOUNT_TICKNESS + 0.1, center=true);
}

difference() {
	translate([0, -MOUNT_HEIGHT/2-0.1, 0])
		cube(size=[MOUNT_WIDTH, MOUNT_HEIGHT, MOUNT_TICKNESS], center=true);
	translate([0, -SERVO_HEIGHT/2, 0])
		servo();
	mountScrews();
}