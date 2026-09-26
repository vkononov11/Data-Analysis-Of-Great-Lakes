#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define NUM_YEARS 31
#define NUM_DAYS 365
#define NUM_LAKES 6
#define MAX_LINE 4096

const char *LAKE_SUFFIX[NUM_LAKES] = {"s", "o", "e", "h", "m", "c"};
const char *LAKE_NAME[NUM_LAKES] = {"Superior", "Ontario", "Erie", "Huron", "Michigan", "St. Clair"};

double temps[NUM_LAKES][NUM_DAYS][NUM_YEARS];

void dayToMonthDay(int day, char *buf){
	int daysInMonth[] =  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	const char *monthNames[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
	int m = 0, d = day; 
	while(m < 12 && d > daysInMonth[m]){
		d -= daysInMonth[m];
		m++;
	}
	sprintf(buf, "%s %d", monthNames[m], d);
}

void swap(double *a, double *b){
	double temp = *a;
	*a = *b;
	*b = temp;
}

void bubbleSort(double arr[], int size){
    for(int i = 0; i < size -1; i++){
        for(int j = 0; j<size -1-i;j++){
            if(arr[j] > arr[j+1]){
                swap(&arr[j], &arr[j+1]);
            }
        }
    }
}

double findMedian(double arr[], int start, int end){
    int n = end - start + 1;
    int mid = start + n/2;
 
    if(n % 2 == 1){
        return arr[mid];
    }else{
        return (arr[mid - 1] + arr[mid])/2.0;
    }
}

void findQuartiles(double arr[], int size, double *Q1, double *Q2, double *Q3){
	
	bubbleSort(arr, size);

	*Q2 = findMedian(arr, 0, size - 1);
	int mid = size/2;

	if(size % 2 == 0){
		*Q1 = findMedian(arr, 0, mid - 1);
		*Q3 = findMedian(arr, mid, size - 1);
	}
	else {
		*Q1 = findMedian(arr, 0, mid - 1);
		*Q3 = findMedian(arr, mid + 1, size - 1);
	}
}

double mean(double arr[], int size){
    double sum = 0;
 
    for(int i = 0; i < size; i++){
        sum += arr[i];
    }
    return sum/size;
}


int loadLake(int lakeIdx){
	char filename[64];
	sprintf(filename, "all_year_glsea_avg_%s_C.csv", LAKE_SUFFIX[lakeIdx]);

	FILE *fp = fopen(filename, "r");
	if(!fp){
		fprintf(stderr, "ERROR: Cannot open file '%s'\n", filename);
		return 0;
	}

	char line[MAX_LINE];
	while(fgets(line, sizeof(line), fp)){
		char *token = strtok(line, " ,\t\r\n");
		if(!token)
			continue;
		int day = atoi(token) - 1;
		if(day < 0 || day >= NUM_DAYS)
			continue;

		int yr = 0;
		while((token = strtok(NULL, " ,\t\r\n")) != NULL && yr < NUM_YEARS){
			temps[lakeIdx][day][yr] = atof(token);
			yr++;
		}
	}
	fclose(fp);
	return 1;
}


void printSeparator(char ch, int n){
	int i;
	for(i=0; i<n; i++){
		putchar(ch);
	}
	putchar('\n');
}


void analyzeQuartileAndOutliers(int lakeIdx){
	int day, yr;
	double arr[NUM_YEARS];
	double Q0, Q1, Q2, Q3, Q4, IQR, LB, UB; 
	double q1, q2, q3;

	printf("\n");
	printSeparator('=', 72);
	printf("  LAKE: %s - Quartile Analsysis (all 365 days)\n", LAKE_NAME[lakeIdx]);
	printSeparator('=', 72);
	
	printf("%-6s %-12s %-8s %-8s %-8s %-8s %-8s %-8s %-8s\n", 
		"Day", "Date", "Q0", "Q1", "Q2", "Q3", "Q4", "IQR", "Mean");
	printSeparator('=', 72);

	for(day = 0; day < NUM_DAYS; day++){
		for(yr = 0; yr<NUM_YEARS; yr++){
			arr[yr] = temps[lakeIdx][day][yr];
		}

		double sorted[NUM_YEARS];
		for(yr=0; yr < NUM_YEARS; yr++){
			sorted[yr] = arr[yr];
		}
		bubbleSort(sorted, NUM_YEARS);
		Q0 = sorted[0];
		Q4 = sorted[NUM_YEARS - 1];

		findQuartiles(arr, NUM_YEARS, &q1, &q2, &q3);
		Q1 = q1;
		Q2 = q2;
		Q3 = q3;
		IQR = Q3 - Q1;

		char dateStr[32];
		dayToMonthDay(day + 1, dateStr);

		printf("%-6d %-12s %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f\n",
			day+1, dateStr, Q0, Q1, Q2, Q3, Q4, IQR, mean(arr, NUM_YEARS));
	}

	printf("\n");
	printSeparator('=', 72);
	printf(" LAKE: %s - Outliers by Year\n", LAKE_NAME[lakeIdx]);
	printSeparator('=', 72);

	int outlierCount[NUM_YEARS];
	double outlierVals[NUM_YEARS][NUM_DAYS];

	for(yr = 0; yr < NUM_YEARS; yr++){
		outlierCount[yr] = 0;
	}

	int anyOutlier = 0;
	for(day = 0; day < NUM_DAYS; day++){
		for(yr=0; yr<NUM_YEARS; yr++){
			arr[yr] = temps[lakeIdx][day][yr];
		}

		findQuartiles(arr, NUM_YEARS, &q1, &q2, &q3);
		IQR = q3- q1;
		LB = q1 - 1.5 * IQR;
		UB = q3 + 1.5 * IQR;

		for(yr=0; yr<NUM_YEARS; yr++){
			double v = temps[lakeIdx][day][yr];
			if ( v < LB || v > UB){
				char dateStr[32];
				dayToMonthDay(day + 1, dateStr);
				printf(" Year %d | Day %3d (%-12s) | Temp: %6.2f degC | [LB=%.2f, UB=%.2f]\n", 1995+yr, day+1, dateStr, v, LB, UB);
				outlierVals[yr][outlierCount[yr]] = v;
				outlierCount[yr]++;
				anyOutlier = 1;
			}
		}
	}

	if(!anyOutlier)
		printf("  No outliers detected for lake %s\n", LAKE_NAME[lakeIdx]);

	printf("\n Outlier count summary for Lake %s:\n", LAKE_NAME[lakeIdx]);
	printSeparator('-', 72);
	printf("  %-8s   %-7s   %s\n", "year", "Count", "Outlier Values (degC)");
	printSeparator('-', 72);
	for(yr=0; yr<NUM_YEARS; yr++){
		printf("   %-8d    %-7d (", 1995+yr, outlierCount[yr]);
		if(outlierCount[yr] == 0){
			printf("none");
		}
		else {
			int k;
			for(k=0; k<outlierCount[yr];k++){
				printf("%-.2f", outlierVals[yr][k]);
				if(k < outlierCount[yr] - 1)
					printf(", ");
			}
		}
		printf(")\n");
	}
	

}


void coldestWarmestDay(int lakeIdx){
	int day, yr; 
	double dayAvg[NUM_DAYS];
	double arr[NUM_YEARS];

	for(day=0; day < NUM_DAYS; day++){
		for(yr=0; yr < NUM_YEARS; yr++){
			arr[yr] = temps[lakeIdx][day][yr];
		}
		dayAvg[day] = mean(arr, NUM_YEARS);
	}

	int coldDay = 0, warmDay = 0;
	for(day = 1; day < NUM_DAYS; day++)
	{
		if(dayAvg[day] < dayAvg[coldDay])
		{
			coldDay = day;
		}
		if(dayAvg[day] > dayAvg[warmDay])
		{
			warmDay = day;
		}
	}

	char coldStr[32], warmStr[32];
	dayToMonthDay(coldDay + 1, coldStr);
	dayToMonthDay(warmDay + 1, warmStr);

	printf(" %-10s | Coldest : Day %3d (%-14s) Avg=%6.2fdegC | Warmest: Day %3d (%-14s) Avg = %6.2fdegC\n", 
		LAKE_NAME[lakeIdx], 
		coldDay + 1, coldStr, dayAvg[coldDay], 
		warmDay + 1, warmStr, dayAvg[warmDay]);
}


void summerAverageRanked()
{
	int SUMMER_START = 171; 
	int SUMMER_END = 264;
	int lk, day, yr;
	double summerAvg[NUM_LAKES];

	for(lk=0; lk < NUM_LAKES; lk++)
	{
		double sum = 0.0;
		int cnt = 0; 
		for(day = SUMMER_START; day <= SUMMER_END; day++)
		{
			for(yr=0; yr < NUM_YEARS; yr++)
			{
				sum += temps[lk][day][yr];
				cnt++;
			}
		}
		summerAvg[lk] = sum / (double)cnt;
	}

	int order[NUM_LAKES];
	int i, j;
	for(i=0; i < NUM_LAKES; i++)
		order[i] = i;

	for(i=0; i < NUM_LAKES - 1; i++)
	{
		for(j=0; j < NUM_LAKES - 1; j++)
		{
			if(summerAvg[order[j]] < summerAvg[order[j + 1]])
			{
				int tmp = order[j];
				order[j] = order[j+1];
				order[j+1] = tmp;
			}
		}
	}

	printf("\n");
	printSeparator('=', 60);
	printf(" Requirement 6: Summer Average (Day 172 - 265, All Years) \n");
	printf(" Lakes ranked warmest to coldest\n");
	printSeparator('=', 60);
	printf(" %-4s %-12s %s\n", "Rank", "Lake", "Summer Avg Temp (degC)");
	printSeparator('-', 40);
	for(i=0; i<NUM_LAKES; i++)
	{
		int lk2 = order[i];
		printf(" %-4d %-12s %.4f\n", 
			i+1, LAKE_NAME[lk2], summerAvg[lk2]);
	}
}


int main(){
	int lk;

	printf("Loading data files...\n");
	for(lk=0; lk < NUM_LAKES; lk++){
		if(!loadLake(lk)){
			fprintf(stderr, "Aborting due to missing file\n");
			return EXIT_FAILURE;
		}
		printf("  Loaded: all_year_glsea_avg_%s_C.csv (%s)\n", LAKE_SUFFIX[lk], LAKE_NAME[lk]);
	}
	printf("All files loaded successfully.\n\n");
	printf("\n");
	
	for(lk=0; lk<NUM_LAKES; lk++){
		analyzeQuartileAndOutliers(lk);
	}
	printf("\n");
	printSeparator('#', 72);
	printf("## REQUIREMENT 5: Coldest and warmest day (30 year average) \n");
	printSeparator('#', 72);
	printf("\n");
	printf(" %-10s | %-34s | %-34s\n",
		"Lake", "Coldest Day", "Warmest Day");
	printSeparator('#', 72);
	for(lk = 0; lk < NUM_LAKES; lk++)
	{
		coldestWarmestDay(lk);
	}

	summerAverageRanked();

	printf("\n");
	printSeparator('#', 72);
	printf(" Program complete.\n");
	printSeparator('#', 72);
	printf("\n");


	return 0;
}





