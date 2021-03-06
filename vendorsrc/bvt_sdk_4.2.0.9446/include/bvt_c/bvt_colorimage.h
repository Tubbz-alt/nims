/*
    This file has been generated by bvtidl.pl. DO NOT MODIFY!
*/
#ifndef __BVTCOLORIMAGE_H__
#define __BVTCOLORIMAGE_H__

#include <bvt_c/bvt_retval.h>

#ifndef DOXY_IGNORE
#ifdef _WIN32
#	ifdef BUILDING_BVTSDK
#		define BVTSDK_EXPORT __declspec(dllexport)
#	else
#		define BVTSDK_EXPORT __declspec(dllimport)
#	endif
#	if !defined(BVTSDK_NO_DEPRECATE) && _MSC_VER >= 1310
#		define BVTSDK_EXPORT_OBSOLETE BVTSDK_EXPORT __declspec(deprecated("This will be removed in a future version of the BlueView SDK"))
#	else
#		define BVTSDK_EXPORT_OBSOLETE BVTSDK_EXPORT
#	endif
#else
#	if __GNUC__ >= 4
#		define BVTSDK_EXPORT __attribute__ ((visibility ("default")))
#	else
#		define BVTSDK_EXPORT
#	endif
#	if !defined(BVTSDK_NO_DEPRECATE) && ((__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)))
#		define BVTSDK_EXPORT_OBSOLETE BVT_EXPORT __attribute__((__deprecated__("This will be removed in a future version of the BlueView SDK")))
#	else
#		define BVTSDK_EXPORT_OBSOLETE BVTSDK_EXPORT
#	endif
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @file
 * @defgroup BVTColorImage BVTColorImage
 * @brief
 * Store a color image.  The API is nearly identical to MagImage.  The main difference is the 
 * pixel datatype. In ColorImage, each pixel is a single unsigned int. 
 * - Byte 0: Red Value
 * - Byte 1: Green Value
 * - Byte 2: Blue Value
 * - Byte 3: Alpha Value
 * @{
*/

/** Opaque type for BVTColorImage object instances */
typedef struct BVTOpaqueColorImage* BVTColorImage;

/** Create a BVTColorImage object */
BVTSDK_EXPORT BVTColorImage BVTColorImage_Create();
/** Destroy a BVTColorImage object
 *  @param obj Object pointer
*/
BVTSDK_EXPORT void BVTColorImage_Destroy(BVTColorImage obj);

/** Return the value of the pixel at (row, col)
 * @param self Object pointer
 * @param row Requested row 
 * @param col Requested col 
 * @param pixel Value of the pixel at (row, col) 
 * @returns pixel

 */
BVTSDK_EXPORT RetVal BVTColorImage_GetPixel(BVTColorImage self, int row, int col, unsigned int* pixel);

/** Set the value of the pixel at (row, col)
 * @param self Object pointer
 * @param row Requested row 
 * @param col Requested col 
 * @param pixel Value of the pixel at (row, col) 
 */
BVTSDK_EXPORT RetVal BVTColorImage_SetPixel(BVTColorImage self, int row, int col, unsigned int pixel);

/** Return a pointer to a row of pixels	
 * @param self Object pointer
 * @param row Requested row index 
 * @param rowPointer pointer to pointer to row
 * @returns rowPointer

 */
BVTSDK_EXPORT RetVal BVTColorImage_GetRow(BVTColorImage self, int row, unsigned int** rowPointer);

/** Return a pointer to the entire image.
 * The image or organized in Row-Major order (just like C/C++).
 * @param self Object pointer
 * @param bitsPointer pointer to pointer to image 
 * @returns bitsPointer

 */
BVTSDK_EXPORT RetVal BVTColorImage_GetBits(BVTColorImage self, unsigned int** bitsPointer);

/** Copy the raw image data to the user specified buffer. See BVTColorImage_GetBits for more info.
 * @param self Object pointer
 * @param data Pointer to a valid buffer 
 * @returns data

 * @param len The size of the buffer pointed to by data in pixels NOT bytes. 
 */
BVTSDK_EXPORT RetVal BVTColorImage_CopyBits(BVTColorImage self, unsigned int data[], unsigned int len);

/** Return the number of boundaries for this image. A boundary defines
 * the limit of data for a transducer, and is always a line.
 * @param self Object pointer
 * @param count The number of boundaries defined for this	image. 
 * @returns count

 */
BVTSDK_EXPORT RetVal BVTColorImage_GetBoundaryCount(BVTColorImage self, int* count);

/** Get the true line segment for the boundary at the specified index.
 * @param self Object pointer
 * @param index The zero-based index of the boundary. 
 * @param x0 The near x-coordinate in meters. 
 * @returns x0

 * @param y0 The near y-coordinate in meters. 
 * @returns y0

 * @param x1 The far x-coordinate in meters. 
 * @returns x1

 * @param y1 The far y-coordinate	in meters. 
 * @returns y1

 */
BVTSDK_EXPORT RetVal BVTColorImage_GetBoundaryLineSegment(BVTColorImage self, int index, float* x0, float* y0, float* x1, float* y1);

/** Get the approximate bearing angle for the boundary at the specified index. A boundary may not necessarily start at the origin, so this
 * approximation may be too imprecise for some applications.
 * @param self Object pointer
 * @param index the zero-based index of the boundary 
 * @param radians the approximate bearing in radians of this boundary 
 * @returns radians

 */
BVTSDK_EXPORT RetVal BVTColorImage_GetBoundaryRadiansApproximate(BVTColorImage self, int index, float* radians);

/** Save the image in PPM (PortablePixMap) format. http://en.wikipedia.org/wiki/Netpbm_format
 * @param self Object pointer
 * @param file_name File name to save to 
 */
BVTSDK_EXPORT RetVal BVTColorImage_SavePPM(BVTColorImage self, const char* file_name);

/** Return the height (in pixels) of this image
 * @param self Object pointer
 * @param height image height
 * @returns height

 */
BVTSDK_EXPORT RetVal BVTColorImage_GetHeight(BVTColorImage self, int* height);

/** Return the width (in pixels) of this image
 * @param self Object pointer
 * @param width image width
 * @returns width

 */
BVTSDK_EXPORT RetVal BVTColorImage_GetWidth(BVTColorImage self, int* width);

/** Return the max range (in meters) to a non-blank pixel in the image
 * @param self Object pointer
 * @param maxRange max range to a non-blank pixel in the image
 * @returns maxRange

 */
BVTSDK_EXPORT RetVal BVTColorImage_GetMaxRangeOfPixel(BVTColorImage self, float* maxRange);

/** 
 * @param self Object pointer
 * @param type Image type constant (IMAGETYPE_*) 
 * @returns type

 */
BVTSDK_EXPORT RetVal BVTColorImage_GetImageType(BVTColorImage self, int* type);

/** Return the range resolution of this image.
 * The resolution is returned in meters per pixel row.
 * @param self Object pointer
 * @param resolution image range resolution
 * @returns resolution

 */
BVTSDK_EXPORT RetVal BVTColorImage_GetRangeResolution(BVTColorImage self, double* resolution);

/** Only valid for R-Theta images.
 * Returns the bearing resolution, in degrees per pixel column.
 * @param self Object pointer
 * @param resolution image bearing resolution
 * @returns resolution

 */
BVTSDK_EXPORT RetVal BVTColorImage_GetBearingResolution(BVTColorImage self, double* resolution);

/** Retrieve the image row of the origin.
 * In most cases the origin row will be outside of the image boundaries (i.e., negative). The origin is the 'location' (in pixels) of the sonar head in image plane.
 * @param self Object pointer
 * @param row pixel row
 * @returns row

 */
BVTSDK_EXPORT RetVal BVTColorImage_GetOriginRow(BVTColorImage self, int* row);

/** Retrieve the image column of the origin.
 * The origin is the 'location' (in pixels) of the
 * sonar head in image plane.
 * @param self Object pointer
 * @param column pixel column
 * @returns column

 */
BVTSDK_EXPORT RetVal BVTColorImage_GetOriginCol(BVTColorImage self, int* column);

/** Retrieve the range (from the sonar head) of the specified pixel (in meters)
 * @param self Object pointer
 * @param row pixel row 
 * @param col pixel col 
 * @param range range to given pixel 
 * @returns range

 */
BVTSDK_EXPORT RetVal BVTColorImage_GetPixelRange(BVTColorImage self, int row, int col, double* range);

/** Retrieve the bearing relative to the sonar head of the specified pixel
 * @param self Object pointer
 * @param row pixel row 
 * @param col pixel col 
 * @param bearing bearing to given pixel 
 * @returns bearing

 */
BVTSDK_EXPORT RetVal BVTColorImage_GetPixelRelativeBearing(BVTColorImage self, int row, int col, double* bearing);

/** Return the pixel coordinate at the specified physical location.
 * The returned coordinate may be outside this image's bounds.
 * @param self Object pointer
 * @param range range in meters 
 * @param bearing bearing in radians 
 * @param x pixel index in the X-axis 
 * @returns x

 * @param y pixel index in the Y-axis 
 * @returns y

 */
BVTSDK_EXPORT RetVal BVTColorImage_GetPixelCoordinateAtRangeBearing(BVTColorImage self, float range, float bearing, int* x, int* y);

/** Return the pixel coordinate at the specified physical location in the sonar's frame.
 * The returned coordinate may be outside this image's bounds.
 * @param self Object pointer
 * @param x_meters X distance in meters 
 * @param y_meters Y distance in meters 
 * @param x pixel index in the X-axis 
 * @returns x

 * @param y pixel index in the Y-axis 
 * @returns y

 */
BVTSDK_EXPORT RetVal BVTColorImage_GetPixelCoordinateAtXY(BVTColorImage self, float x_meters, float y_meters, int* x, int* y);

/** Return the minimum angle for the sonar's imaging field of view. 
 * The angle is returned in degrees and referenced with respect to sonar head boresight (clockwise-positive).
 * @param self Object pointer
 * @param angle min. angle
 * @returns angle

 */
BVTSDK_EXPORT RetVal BVTColorImage_GetFOVMinAngle(BVTColorImage self, float* angle);

/** Return the maximum angle for the sonar's imaging field of view. 
 * The angle is returned in degrees and referenced with respect to sonar head boresight (clockwise-positive).
 * @param self Object pointer
 * @param angle max. angle
 * @returns angle

 */
BVTSDK_EXPORT RetVal BVTColorImage_GetFOVMaxAngle(BVTColorImage self, float* angle);

/** @}*/

#ifdef __cplusplus
}
#endif

#endif
