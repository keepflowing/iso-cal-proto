#include <stdio.h>
#include <sys/ioctl.h> 
#include <unistd.h>

int LENGTH[] = {
    31, 28, 31, 30, 31, 30, 
    31, 31, 30, 31, 30, 31
};

int string_length(const char *str) {
    int length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}

int days_passed(int currMonth) {
    int sum = 0;
    for(int i = 0; i < currMonth; i++) {
	sum += LENGTH[i];
    }
    return sum;
}

char is_leap_year(int year) {
    return year % 4 == 0 && year % 100 != 0 || year % 400 == 0;
}

int leap_years_before(int year) {
    return (year / 400) - (year / 100) + (year / 4) - is_leap_year(year);
}

const unsigned char MONTH_WIDTH = 25;

const char* MONTHS[] = {
    "January", "February", "March", 
    "April", "May", "June", 
    "July", "August", "September", 
    "October", "November","December"
};

const char* DAYS[] = {
    "Mo",
    "Tu",
    "We",
    "Th",
    "Fr",
    "Sa",
    "Su"
};

void print_headers(int start, int cols) {
    printf("\n");
    for(int i = 0; i < cols; i++) {
	if(start + i >= 12) break;

	int p = MONTH_WIDTH - string_length(MONTHS[start + i]);
	printf("%*s%s%*s", p/2, "", MONTHS[start + i], (p - p/2), "");
    }
    printf("\n");

    for(int i = 0; i < cols; i++) {
	if(start + i >= 12) break;
	
	printf(" w ");
	for(int j = 0; j < 7; j++) {
	    printf("%s ", DAYS[j]);
	}
	printf(" ");
    }
    printf("\n");
} 

int weekOffset = 0;
int get_week(int* currDate, int month, int year) {
    int daysSinceZero = 365 * (year-1) + leap_years_before(year);
    int firstDay = daysSinceZero % 7;
    int daysPassed = days_passed(month) + *currDate;
    
    int week = (daysPassed + firstDay - 1) / 7 + 1;

    if(week == 1 && firstDay > 3) {
	if(firstDay == 4) week = 53;
	else if(firstDay == 5) {
	    if(is_leap_year(year-1)) week = 53;
	    else week = 52;
	}
	else week = 52;
	weekOffset = -1;
	return week;
    }
    if(week + weekOffset == 53 && *currDate >= 29) return 1;

    return week + weekOffset;
}

void print_row(int firstDay, int* currDate, int month, int year) {
    if(*currDate > LENGTH[month]) {
	printf("%*s", MONTH_WIDTH, "");
	return;
    }
    int i = 0;
    int week = get_week(currDate, month, year);
    printf("%2d ", week);
    for(;i < firstDay; i++) printf("   ");
    for(;i < 7; i++) {
	if(*currDate <= LENGTH[month]) {
	    printf("%2d ", *currDate);
	    (*currDate)++;
	}
	else printf("   ");
    }
    printf(" ");
}

int days_left(int start, int currDates[], int cols) {
    for(int i = 0; i < cols; i++) {
	if (start + i >= 12) break;
	if(currDates[start + i] <= LENGTH[start + i]) return 1;
    }
    return 0;
}

void print_dates(int start, int year, int cols) {
    int daysSinceZero = 365 * (year-1) + leap_years_before(year);
    int firstDay = daysSinceZero % 7;
    int currDates[12] = {1,1,1,1,1,1,1,1,1,1,1,1};
    int firstRow = 1;
    while(days_left(start, currDates, cols) != 0) {
	for(int i = 0; i < cols; i++) {
	    if(start+i >= 12) break;
	    if(firstRow == 1) {
		int daysPassed = days_passed(start+i);
		firstDay = (daysSinceZero % 7 + daysPassed) % 7;
		print_row(firstDay, &currDates[start+i], start+i, year);
	    }
	    else {
		print_row(0, &currDates[start+i], start+i, year);
	    }
	    
	}
	firstRow = 0;
	printf("\n");
    }
}

void print_calendar(int year, int cols) {
    if(is_leap_year(year)) LENGTH[1] = 29;
    for(int i = 0; i < 12; i += cols) {
	print_headers(i, cols);
	print_dates(i, year, cols);
    }
}

int main() {
    int year;
    
    while(1) {
	printf("Enter year: ");
	if(scanf("%d", &year)) {
	    char c = getchar();
	    if(year < 1) {
		printf("Year must be >= 1\n");
		continue;
	    } else if(c == '\n') {
		break;
	    } else {
		printf("Floating points are not allowed.\n");
	    }
	} else {
	    printf("Non-number input is not allowed.\n");
	}
	while(getchar() != '\n');
    }
   
    struct winsize w; 
    int cols;

    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != 0) {
	cols = MONTH_WIDTH;
    } else {
	cols = w.ws_col / MONTH_WIDTH;
    }
    
    for(int i = 0; i < cols; i++) {
	printf("-------------------------");
    }
    printf("\n");
    printf("%*s%4d\n", ((MONTH_WIDTH * cols)/2-2), "", year);

    print_calendar(year, cols);

    return 0;
}
