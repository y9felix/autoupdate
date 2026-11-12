enum {
	SLEEPSECS    = 30,
	IDLESECS     = 14400,
	RETRYSECS    = 900,
	COOLDOWNSECS = 64800
};

#define ACPATH    "/sys/class/power_supply/AC/online"
#define LIDPATH   "/proc/acpi/button/lid/LID/state"
#define STAMPPATH "/var/lib/autoupdate.stamp"

#define CMDREBOOT { "/usr/bin/systemctl", "reboot", NULL }
#define CMDUPDATE { "/usr/bin/pacman", "-Syu", "--noconfirm", "--noprogressbar", NULL }
