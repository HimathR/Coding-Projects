attach(Transport)
attach(TransportCopy)
#variables: Mode (can be categorised cost), Age, Frequency

basicStats(Mode$CatPriec)

x<-(TransportCopy$CatPrice[TransportCopy$mode == "Public"])

x = Transport$Cost
# Create A Histogram with box plot using "PackHV" library
hist_boxplot(x, main = "Histogram of Transport Cost",
             xlab = "Cost ($/week)",
             freq = FALSE, # Frequency must be changed to density for a density curve
             col = "lightskyblue"
)
# Create a density curve above histogram with box plot
s = sd(x); m = mean(x); curve(dnorm(x, mean = m, sd = s),
                              col = "brown4",
                              add = TRUE)


# Make numerical cost variable into a categorical one
CostLevels <- cut(Cost, breaks = c(35.84,53.04,70.24,87.45)) # Add category boundaries
levels(CostLevels) <- c("Low", "Medium", "High") # Name each level

# make a copy of data
TransportCopy <- Transport # Copy integers in Transport to TransportCopy
TransportCopy$CatPrice <- CostLevels # Create new column called CatPrice 
#This will now have the price for transport as a categorical variable

# Create a contingency table for the 2 categorical variables investigated
table(TransportCopy$CatPrice)

chisq.test(CatPrice, mode) # Run a chi squared test

#Create Bar Plot
attach(TransportCopy)
plot(TransportCopy$mode, TransportCopy$CatPrice, 
     main = "Stacked Bar chart of Transport Mode vs Categorical Cost", 
     xlab = "Mode", 
     ylab = "Cost (Categorical)")




detach(Transport)
detach(TransportCopy)

