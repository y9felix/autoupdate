static const unsigned int sleep_secs = 30;
static const time_t idle_secs        = 14400;
static const time_t retry_secs       = 900;
static const time_t cooldown_secs    = 64800;

static const char *ac_path    = "/sys/class/power_supply/AC/online";
static const char *lid_path   = "/proc/acpi/button/lid/LID/state";
static const char *stamp_path = "/var/lib/autoupdate.stamp";

static char *const cmd_reboot[] = { "/usr/bin/systemctl", "reboot", NULL };
static char *const cmd_update[] = {
	"/usr/bin/pacman", "-Syu", "--noconfirm", "--noprogressbar", NULL
};
