import math
import time
import cv2 as cv
import numpy as np
import matplotlib.pyplot as plt

# HELPER FUNCTIONS
def show_image(image, title):  # For showing intermediate images created and final image
    plt.figure()
    plt.title(title)
    plt.imshow(image)
    plt.show()

# For overlaying the calculated lines using Hough
def add_lines(image, lines):
    if lines is None:  # Make sure there are lines to draw
        return
    image_copy = np.copy(image)
    blank_image = np.zeros(
        (
            image_copy.shape[0],
            image_copy.shape[1],
            3
        ),
        dtype=np.uint8
    )
    # Draw each line on top of the blank image defined above
    for line in lines:
        for x1, y1, x2, y2 in line:
            cv.line(blank_image, (x1, y1), (x2, y2), [255,0,0], 5)
    # Merge the blank image with lines and the original image
    image_copy = cv.addWeighted(image, 1, blank_image, 1, 0)
    return image_copy


# For getting the left side lines of an image
def left_lines(left_x, left_y, min_y, max_y):
    left = np.poly1d(np.polyfit(
        left_y,
        left_x,
        deg=1
    ))
    candidateL_P1 = int(left(max_y))
    candidateL_P2 = int(left(min_y))
    return candidateL_P1, candidateL_P2

selective = False  # If False, selective line choosing won't be done

# For getting the right side lines of an image
def right_lines(right_x, right_y, min_y, max_y):
    right = np.poly1d(np.polyfit(
        right_y,
        right_x,
        deg=1
    ))
    candidateR_P1 = int(right(max_y))
    candidateR_P2 = int(right(min_y))
    return candidateR_P1, candidateR_P2


# Driver function of hough for selecting line candidates
def overlay_best_lines(image, hough_lines, selective):
    left_x = []
    left_y = []
    right_x = []
    right_y = []

    for line in hough_lines:
        for x1, y1, x2, y2 in line:
            # Calculate gradient for each line found
            if (x2 - x1) == 0:  # Check for divide by 0 errors
                continue
            gradient = (y2 - y1)/(x2 - x1)
            if math.fabs(gradient) < 0.5 or gradient == 0:
                continue
            # Divide slopes into two groups
            if not selective:
                if gradient < 0:  # Negative Gradient = Left Side
                    # print("LEFT ", degree)
                    left_x.extend([x1, x2])
                    left_y.extend([y1, y2])
                else:  # Positive Gradient = Right Side
                    # print("RIGHT ", degree)
                    right_x.extend([x1, x2])
                    right_y.extend([y1, y2])
            else:
                degree = round((math.degrees(math.atan(gradient))), 1)
                if gradient <= 0 and degree > -65:  # Negative Gradient = Left Side
                    # print("LEFT ", degree)
                    left_x.extend([x1, x2])
                    left_y.extend([y1, y2])
                elif gradient > 0 and degree < 45:  # Positive Gradient = Right Side
                    # print("RIGHT ", degree)
                    right_x.extend([x1, x2])
                    right_y.extend([y1, y2])

    min_y = int(image.shape[0] * (3 / 5))  # Below image horizon
    max_y = image.shape[0]  # Bottom of image

    noleftlines = False
    norightlines = False

    if left_x and left_y is not None:
        best_line_leftP1, best_line_leftP2 = left_lines(left_x, left_y, min_y, max_y)
    else:
        noleftlines = True  # If only the left side lines of an image can be found
    if right_x and right_y is not None:
        best_line_rightP1, best_line_rightP2 = right_lines(
            right_x, right_y, min_y, max_y)
    else:
        norightlines = True  # If only the right side lines of an image can be found

    if noleftlines and norightlines:  # If no lines could be found in the image, terminate here
        print("No Lines Found, Quitting...")
        return 0
    elif noleftlines:  # draw only the right side lines found
        line_image = add_lines(
            image,
            [[
                [best_line_rightP1, max_y, best_line_rightP1, min_y],
                [best_line_rightP1, max_y, best_line_rightP2, min_y],
            ]]
        )
    elif norightlines:  # draw only the left side lines found
        line_image = add_lines(
            image,
            [[
                [best_line_leftP1, max_y, best_line_leftP2, min_y],
                [best_line_leftP1, max_y, best_line_leftP1, min_y],
            ]]
        )
    else:  # draw both the left AND right side lines
        best_line_leftP1, best_line_leftP2 = left_lines(left_x, left_y, min_y, max_y)
        best_line_rightP1, best_line_rightP2 = right_lines(
            right_x, right_y, min_y, max_y)
        line_image = add_lines(
            image,
            [[
                [best_line_leftP1, max_y, best_line_leftP2, min_y],
                [best_line_rightP1, max_y, best_line_rightP2, min_y],
            ]]
        )
    if (norightlines or noleftlines):
        return line_image
    else:
        vertices = [
            (best_line_leftP1, max_y),
            (best_line_leftP2, min_y),
            (best_line_rightP2, min_y),
            (best_line_rightP1, max_y)
        ]
        crop_image = get_ROI(line_image, (0, 0, 255),
            np.array([vertices], np.int32)
        )
        line_image = line_image+crop_image
        return line_image


def get_ROI(image, colour, vertices):  # Function for getting the region of interest
    mask = np.zeros_like(image)
    cv.fillPoly(mask, vertices, colour)
    region_of_interest = cv.bitwise_and(image, mask)
    return region_of_interest


def main_process(image, ug, sp):
    # Pre-Processing Parameters
    use_gaussian = ug  # If False, use bilateral. If True, use Gaussian
    show_process = sp # If False, don't show intermediate steps of image
    
    # Start Lane Detection
    if image is None:
        print("No Image Detected! Try Again")
        exit()

    print("-=+=-")
    print('Starting Lane Detection With', end=' ')
    print('GAUSSIAN Filtering' if use_gaussian else 'BILATERAL Filtering')
    print('All intermediate steps will be shown\n' if show_process else '', end='')

    start = time.time()   # Start Timer
    # Get some basic image information
    image = cv.cvtColor(image, cv.COLOR_BGR2RGB)

    # Convert Image To Grayscale
    gray_image = cv.cvtColor(image, cv.COLOR_RGB2GRAY)
    gray_image = cv.cvtColor(gray_image, cv.COLOR_BGR2RGB)

    # Get Filtered Image (with either gaussian or bilateral filter)
    if use_gaussian:
        filtered_image = cv.GaussianBlur(gray_image, (5, 5), 0)
    else:
        filtered_image = cv.bilateralFilter(gray_image, 5, 5, 0)

    # Get Canny Edges
    canny_image = cv.Canny(filtered_image, 100, 200)

    # Define vertices for the image
    height = image.shape[0]
    width = image.shape[1]
    vertices = [(0, height), (width / 2, height / 2), (width, height)]
    # vertices = [(0, height), (width / 3, height / 3), (width, height)] <-- For a wider based triangle
    # susceptible to more noise but can sometimes lead to better fitting (not always)
    vertices = np.array([vertices], np.int32)

    # Get Region Of Interest
    cropped_image = get_ROI(canny_image, 255, vertices)

    # Get Hough Lines - Toggle Parameters Below
    Hough_Rho = 6
    Hough_theta = np.pi/60
    Hough_threshold = 100
    Hough__lines = np.array([])
    linelen = 40
    linegap = 50

    hough_lines = cv.HoughLinesP(cropped_image, rho=Hough_Rho, theta=Hough_theta,
                                threshold=Hough_threshold, lines=Hough__lines,
                                 minLineLength=linelen, maxLineGap=linegap)

    # Draw the relevant lines
    if hough_lines is not None:
        final_image = overlay_best_lines(image, hough_lines, selective)
    else:
        print("No lines could be detected in the image...")
        exit()

    end = time.time()  # End Timer
    total = end - start
    print("Time Taken For This Frame ", round(total, 3), "Seconds")
    print("-=+=-")
    
    # Display Results
    if show_process:
        show_image(gray_image, "Grayscale Image")
        if use_gaussian:
            show_image(filtered_image, "Filtered Image (Gaussian)")
        else:
            show_image(filtered_image, "Filtered Image (Bilateral)")
        show_image(cropped_image, "Cropped Image")
        show_image(canny_image, "Canny Image")
    return final_image

def main():  # DRIVER FUNCTION
    path = input("Enter Image Path: ") # read in an image
    while path != 'quit':
        image = cv.imread(path)

        gaussian_image = main_process(image, True, False)
        bilateral_image = main_process(image, False, False)
        fig = plt.figure(figsize=(10, 3))

        fig.add_subplot(1, 3, 1)
        plt.title("Original")
        plt.imshow(cv.cvtColor(image, cv.COLOR_RGB2BGR))
        fig.add_subplot(1, 3, 2)
        plt.title("Gaussian")
        plt.imshow(gaussian_image)
        fig.add_subplot(1, 3, 3)
        plt.title("Bilateral")
        plt.imshow(bilateral_image)
        plt.suptitle("Gaussian Vs Bilateral")
        plt.show()
        path = input("Enter Image Path: ")
    
    exit(0)


main()
