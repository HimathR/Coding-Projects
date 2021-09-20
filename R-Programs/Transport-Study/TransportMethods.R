#Transport <- get_my_data(5209861, "Transport")

attach(Transport)

table(mode, sch) # Create a Table

library(ggplot2) # Install GGPlot2 library 
ggplot(data = Transport) + # specify data 
  aes(x = Cost, y = ..density..) + 
  geom_histogram(binwidth = 6, fill = "lightblue", colour = "black") + # bar colours & outline colors
  geom_density() + facet_grid(mode ~ sch) # include density plot


ggplot(data = Transport) + 
  aes(x=sch, y = Cost, fill = sch) + 
  geom_boxplot(varwidth = TRUE) + # specify to create boxplot
  facet_wrap(~mode) + theme(legend.position = "bottom") + # formatting
  scale_fill_manual(values = c("cyan1", "cyan3", "cyan4")) # colours for each variable


#Using the Cost fails normality of residuals
#Run Inverse Transformation on Data
#Run the ANOVA test again with the transformed variable "InvCost)
InvCost = 1/(Cost) #transforming Cost using inverse (1/x)
linmodel = lm(InvCost ~ sch*mode, data = Transport) # Create Linear Model
avmodel = aov(linmodel) # Run ANOVA test on linear model
summary(avmodel) # Summarize model

plot(avmodel, 1) # 1 = Residuals vs Fitted Graph for Variance
plot(avmodel, 2) # 2 = QQPlot for Normality


library(car) # Install car library for Levene's test
leveneTest(linmodel) # Variance Test

res=avmodel$residuals # Isolate residuals from model
shapiro.test(res) # Normality Test code

library(psych) # Install Psych Library
Results = describeBy(Cost ~ mode + sch, # Cost with regards to mode and sch
           mat = TRUE, # For better formatting
           data = Transport, digits = 2) # Generate table 

library(agricolae) # Post Hoc Test
posthoc = LSD.test(avmodel, c("mode"), console = TRUE) # Least Significant Difference Test
# "mode" = main effect test is being done on
plot(posthoc, xlab = "Mode") # Plot PostHoc results

detach(Transport)


#Export to Excel
library(writexl) # Install WriteXL library
# include correct path for document to be written to
write_xlsx(Results, "C:\\Users\\himat\\OneDrive\\Desktop\\EDA_petrol.xlsx") 


# Generate Citations
citation("psych")
citation("agricolae")
citation("car")
citation("writexl")

