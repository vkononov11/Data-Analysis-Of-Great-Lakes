# Data-Analysis-Of-Great-Lakes

# Run the code:

gcc LakesDataAnalysis.c -o lakes
./lakes

# About: 

This project is a C program that analyzes historical water temperature data for six Great Lakes: Superior, Ontario, Erie, Huron, Michigan, and St. Clair.

The program reads temperature data from multiple CSV files containing daily measurements across 31 years. It processes the datasets and performs statistical analysis to identify temperature patterns, seasonal differences, and unusual observations.

The analysis includes:

- Reading and processing temperature data from multiple CSV files
- Calculating the mean temperature for each calendar day
- Calculating quartiles (Q1, Q2, and Q3) and the interquartile range (IQR)
- Detecting temperature outliers using the 1.5 × IQR method
- Identifying the historically coldest and warmest calendar days for each lake
- Calculating average summer temperatures
- Ranking the six lakes from warmest to coldest based on summer averages
- Converting numerical day values into readable calendar dates

## Dataset

The program analyzes 31 years of daily temperature data for six Great Lakes, representing more than 67,000 temperature observations.

Temperature data is stored internally using a three-dimensional array organized by lake, day, and year.

## Statistical Methods

The program implements statistical calculations directly in C, including:

- Mean
- Median
- Quartiles
- Interquartile Range (IQR)
- Outlier Detection

An observation is considered an outlier when it falls outside:

`Q1 - 1.5 × IQR`

or

`Q3 + 1.5 × IQR`

## Purpose

The purpose of this project was to practice working with large datasets in C while applying statistical methods to real world temperature data. It demonstrates data parsing, data organization, statistical analysis, and interpretation of historical environmental data.
