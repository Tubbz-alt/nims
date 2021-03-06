from bvt_python.ping import *
from bvt_python.magimage import *
from bvt_python.orientation import *
from bvt_python.navdata import *
from bvt_python.error import *

"""
This file was generated by bvtidl.pl.
Your changes will most likely be lost.
"""

from ctypes import *
import sys
import sdkerror


class Ping(object):
    """
    As its name implies, the Ping object represents the return from 
    a single ping on a particular head. <br>
    A ping is essentially a container for data. As such, after you get
    a ping from the head and extract the data (or save it to a file),
    it is necessary to destroy the ping object to free up memory. <br>
    Each ping may also store navigation data to indicate the position
    and orientation of the vehicle at the time of the ping. <br>
    Each ping may have a video frame associated with it, and saved in
    the same file. These images are typically from a video camera
    mounted near the sonar, such as on a ROV. <br>
    """
    def __init__(self, parent, handle):
        super(Ping, self).__setattr__("_initialized", False)
        self._deleted = False
        if handle is None or type(handle) is not c_void_p:
            raise Exception("Class Ping cannot be directly instantiated")
        else:
            self._handle = handle
        # The parent must not be garbage collected until this instance
        # is deleted
        self._parent = parent
        super(Ping, self).__setattr__("_initialized", True)

    def __del__(self):
        self._deleted = True
        dll.BVTPing_Destroy(self._handle)

    def __setattr__(self, name, value):
        """ Don't allow setting non-existent attributes on this class
        """
        if self._initialized and not hasattr(self, name):
            raise AttributeError("%s instance has no attribute '%s'" % (self.__class__.__name__, name))
        super(Ping, self).__setattr__(name, value)

    def copy(self):
        """
        Creates and returns a copy of this ping.
        \attention Don't forget to call Destroy on the returned copy.
        """
        the_copy_ptr = c_void_p()
        error_code = dll.BVTPing_Copy(self._handle, byref(the_copy_ptr))
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)
        the_copy = Ping(self._parent, the_copy_ptr)
        return the_copy

    def adjust_range(self, new_start, new_stop):
        """
        Change the range window of this ping by removing data from the start or the end of the received singnal.
        \attention This will fail if you try to decrease the existing start range or increase the existing stop range
        """
        error_code = dll.BVTPing_AdjustRange(self._handle, new_start, new_stop)
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)

    @property
    def ping_number(self):
        """
        Return the ping number (i.e., index in file).
        Ping numbers only have meaning if the ping came from a file.
        """
        number = c_int()
        error_code = dll.BVTPing_GetPingNumber(self._handle, byref(number))
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)
        return number.value

    @property
    def head_id(self):
        """
        Return the head ID this ping was captured, or saved with.
        """
        id = c_int()
        error_code = dll.BVTPing_GetHeadID(self._handle, byref(id))
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)
        return id.value

    @property
    def timestamp(self):
        """
        Return the ping's timestamp in seconds since 00:00:00 UTC, January 1, 1970 
        Pings are timestamped using a standard UNIX time stamp. This is 
        a similar value to that returned by the time() C standard library 
        function. In fact, the only difference is the addition of fractional seconds.
        """
        timestamp = c_double()
        error_code = dll.BVTPing_GetTimestamp(self._handle, byref(timestamp))
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)
        return timestamp.value

    @property
    def time_zone_offset(self):
        """
        Return the ping's timestamp's offset in seconds from UTC time.  Add this value to that returned by GetTimestamp() to obtain UTC time.
        """
        offset = c_int()
        error_code = dll.BVTPing_GetTimeZoneOffset(self._handle, byref(offset))
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)
        return offset.value

    @timestamp.setter
    def timestamp(self, sec):
        """
        Set the ping's internal time stamp.  See GetTimestamp() for more information. 
        Note: BlueView strongly recommends that users NOT directly set the time stamp as 
        it is set internally when the ping is actually initiated.  If you are trying to 
        synchronize two systems, it is far better to simply make sure that the system
        clocks are synchronized, as the ping timestamp is created from the 
        PC's internal clock. Network Time Protocol and GPS sources provide highly 
        accurate ways to accomplish this.
        Caution: This also overwrites the ping's recorded time zone offset with the client operating system's time zone offset.
        Call SetTimeZoneOffset after SetTimestamp if you need to maintain the original offset.
        """
        error_code = dll.BVTPing_SetTimestamp(self._handle, sec)
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)

    @time_zone_offset.setter
    def time_zone_offset(self, timeZoneOffset):
        """
        Set the ping's time zone offset in seconds from UTC time.
        """
        error_code = dll.BVTPing_SetTimeZoneOffset(self._handle, timeZoneOffset)
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)

    def get_fov(self, minAngleInDegrees, maxAngleInDegrees):
        """
        Retrieve the angular limits of the ping's field-of-view. Note that the limits
        reported represent only the minimum and maximum angles associated with any
        pixel in the FOV. It is possible (depending on sonar configuration) that not
        all pixels within the reported field-of-view are populated with data.
        """
        error_code = dll.BVTPing_GetFOV(self._handle, byref(minAngleInDegrees), byref(maxAngleInDegrees))
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)

    @property
    def start_range(self):
        """
        Return the ping's the start range in meters.
        """
        range = c_float()
        error_code = dll.BVTPing_GetStartRange(self._handle, byref(range))
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)
        return range.value

    @property
    def stop_range(self):
        """
        Return the stop range in meters.
        """
        range = c_float()
        error_code = dll.BVTPing_GetStopRange(self._handle, byref(range))
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)
        return range.value

    @property
    def sound_speed(self):
        """
        Return the sound speed this ping was recorded at
        """
        sound_speed = c_int()
        error_code = dll.BVTPing_GetSoundSpeed(self._handle, byref(sound_speed))
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)
        return sound_speed.value

    def get_image(self):
        """
        Retrieve an image of this ping, according to the parameters set
        in the head used to get this ping. See Head and MagImage documentation
        for more details.
        """
        img_ptr = c_void_p()
        error_code = dll.BVTPing_GetImage(self._handle, byref(img_ptr))
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)
        img = MagImage(img_ptr)
        return img

    def get_image_xy(self):
        """
        Retrieve an XY-format image of this ping, according to the parameters set
        in the head used to get this ping. Use BVTHead_SetImageSizeXY() to set the size for this image.
        See Head and MagImage documentation for more details.
        """
        img_ptr = c_void_p()
        error_code = dll.BVTPing_GetImageXY(self._handle, byref(img_ptr))
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)
        img = MagImage(img_ptr)
        return img

    def get_image_r_theta(self):
        """
        Retrieve an RTheta-format image of this ping, according to the parameters set
        in the head used to get this ping. Use BVTHead_SetImageWidthRTheta() to set the size for this image.
        See Head and MagImage documentation for more details.
        """
        img_ptr = c_void_p()
        error_code = dll.BVTPing_GetImageRTheta(self._handle, byref(img_ptr))
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)
        img = MagImage(img_ptr)
        return img

    @property
    def max_return_signal(self):
        """
        Returns the percentage of dyn. range (0.0 - 1.0) of the max value in this
        ping. A value of 1.0 indicates clipped data. A value near 0.0 indicates 
        that no targets were illuminated.
        """
        max = c_float()
        error_code = dll.BVTPing_GetMaxReturnSignal(self._handle, byref(max))
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)
        return max.value

    def get_nav_data_copy(self):
        """
        Creates a copy of the navigation data stored with this ping. Note
        that the data is copied out of the ping into the local NavData object,
        a pointer to internal data is not returned. Thus, the NavData object
        may be used after the Ping is destroyed.
        """
        nav_data_ptr = c_void_p()
        error_code = dll.BVTPing_GetNavDataCopy(self._handle, byref(nav_data_ptr))
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)
        nav_data = NavData(nav_data_ptr)
        return nav_data

    def try_get_nav_data_copy(self, default_value):
        """
        Creates a copy of the navigation data stored with this ping. Note
        that the data is copied out of the ping into the local NavData object,
        a pointer to internal data is not returned. Thus, the NavData object
        may be used after the Ping is destroyed.
        """
        nav_data_ptr = c_void_p()
        error_code = dll.BVTPing_GetNavDataCopy(self._handle, byref(nav_data_ptr))
        if (0 != error_code):
            return default_value
        nav_data = NavData(nav_data_ptr)
        return nav_data

    @property
    def has_nav_data(self):
        """
        Returns true if this ping has a valid Navigation data object.
        """
        has_nav_data = c_int()
        error_code = dll.BVTPing_HasNavData(self._handle, byref(has_nav_data))
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)
        return has_nav_data.value > 0

    def set_nav_data(self, nav_data):
        """
        Stores a copy of the navigation data with the other ping data, so the data
        will be saved if the ping is saved to a file.
        """
        error_code = dll.BVTPing_SetNavData(self._handle, nav_data._handle)
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)

    VIDEO_RGB = 0
    VIDEO_JPEG = 1
    def get_video_frame(self, frame, height, width, length, type):
        """
        Returns the video frame associated with this ping.
        The video frame may be in any of the supported image formats.
        Some image formats may already contain parameters such as height
        and width (and more), but valid pointers must be passed in anyway.
        The same pointer can be passed in for multiple parameters, if
        those parameters will not be used. However, they are provided both
        for formats which do not have embedded size information, and so that
        the display window may be created and/or sized without parsing
        the image data.<br>
        NOTE: This function will return BVT_NO_VIDEO_FRAME if there
        is no video frame stored for the ping. <br>
        WARNING: The data buffer must NOT be accessed after the ping object is destroyed,
        as the pointer will no longer point to valid data and will likely crash your application!
        So copy off the data before destroying the Ping object. <br>
        The single value pointers must be pointers to allocated data, not just
        pointer types. For example:<br>
        int height, width, length, type, retval;<br>
        int * frame_ptr;<br>
        retval = GetVideoFrame( frame_ptr, &height, &width, &length, &type );<br>
        """
        error_code = dll.BVTPing_GetVideoFrame(self._handle, byref(frame), byref(height), byref(width), byref(length), byref(type))
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)

    def set_video_frame_jpeg(self, frame, height, width, length):
        """
        Store a JPEG image to save with this ping.
        Note that the height and width values will simply be stored and
        available to read when the frame is retrieved. These have no effect
        on the actual image size (the image will not be resized).
        The length however is very important, as it determines how far from
        the passed image pointer data will be read. An incorrect length could
        result in an application crash.
        """
        error_code = dll.BVTPing_SetVideoFrameJPEG(self._handle, frame, height, width, length)
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)

    def get_positioner_orientation_copy(self):
        """
        Retrieves a copy of the per ping Positioner Orientation data which was stored with this ping.,
        Note that the data is copied out of the ping into the local Orientation object,
        a pointer to internal data is not returned. Thus, the Orientation object
        may be used after the Ping is destroyed. (see SetPositionerOrientation, above, for more
        information on the meaning of the PositionerOrientation data)
        """
        orient_ptr = c_void_p()
        error_code = dll.BVTPing_GetPositionerOrientationCopy(self._handle, byref(orient_ptr))
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)
        orient = Orientation(orient_ptr)
        return orient

    def set_positioner_rotations(self, X_axis_degrees, Y_axis_degrees, Z_axis_degrees):
        """
        Simplified function to set just the rotations from the Ping's PositionerOrientation object.
        The offsets and calibration values will be zero. The source and target frames will be set to
        OR_POSITIONER and OR_HEAD, respectively.
        This function is most useful when only pan/tilt values are available and needed.
        See the SetPositionerOrientation() documentation for more details.
        """
        error_code = dll.BVTPing_SetPositionerRotations(self._handle, X_axis_degrees, Y_axis_degrees, Z_axis_degrees)
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)

    def get_positioner_rotations(self, X_axis_degrees, Y_axis_degrees, Z_axis_degrees):
        """
        Simplified function to get just the rotations from the Ping's PositionerOrientation object.
        """
        error_code = dll.BVTPing_GetPositionerRotations(self._handle, byref(X_axis_degrees), byref(Y_axis_degrees), byref(Z_axis_degrees))
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)

    @property
    def has_signal(self):
        """
        Returns true if this ping has a raw time series.
        """
        has_signal = c_int()
        error_code = dll.BVTPing_HasSignal(self._handle, byref(has_signal))
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)
        return has_signal.value > 0

    @property
    def has_stored_range_profile(self):
        """
        Returns true if this ping has a post-processed profile.
        """
        has_stored_profile = c_int()
        error_code = dll.BVTPing_HasStoredRangeProfile(self._handle, byref(has_stored_profile))
        if (0 != error_code):
            raise sdkerror.SDKError(error_code)
        return has_stored_profile.value > 0


    def get_handle(self):
        """
        SDK object pointer
        """
        return self._handle
        def get_Head(self):
            return self._parent

if "win32" in sys.platform:
    dll_name = "bvtsdk4.dll"
elif "darwin" in sys.platform:
    dll_name = "libbvtsdk.dylib"
else:
    dll_name = "libbvtsdk.so"
dll = CDLL(dll_name)
dll.BVTPing_Destroy.restype = None
dll.BVTPing_Destroy.argtypes = (c_void_p,)
dll.BVTPing_Copy.restype = c_int
dll.BVTPing_Copy.argtypes = (c_void_p, POINTER(c_void_p), )
dll.BVTPing_AdjustRange.restype = c_int
dll.BVTPing_AdjustRange.argtypes = (c_void_p, c_float, c_float, )
dll.BVTPing_GetPingNumber.restype = c_int
dll.BVTPing_GetPingNumber.argtypes = (c_void_p, POINTER(c_int), )
dll.BVTPing_GetHeadID.restype = c_int
dll.BVTPing_GetHeadID.argtypes = (c_void_p, POINTER(c_int), )
dll.BVTPing_GetTimestamp.restype = c_int
dll.BVTPing_GetTimestamp.argtypes = (c_void_p, POINTER(c_double), )
dll.BVTPing_GetTimeZoneOffset.restype = c_int
dll.BVTPing_GetTimeZoneOffset.argtypes = (c_void_p, POINTER(c_int), )
dll.BVTPing_SetTimestamp.restype = c_int
dll.BVTPing_SetTimestamp.argtypes = (c_void_p, c_double, )
dll.BVTPing_SetTimeZoneOffset.restype = c_int
dll.BVTPing_SetTimeZoneOffset.argtypes = (c_void_p, c_int, )
dll.BVTPing_GetFOV.restype = c_int
dll.BVTPing_GetFOV.argtypes = (c_void_p, POINTER(c_float), POINTER(c_float), )
dll.BVTPing_GetStartRange.restype = c_int
dll.BVTPing_GetStartRange.argtypes = (c_void_p, POINTER(c_float), )
dll.BVTPing_GetStopRange.restype = c_int
dll.BVTPing_GetStopRange.argtypes = (c_void_p, POINTER(c_float), )
dll.BVTPing_GetSoundSpeed.restype = c_int
dll.BVTPing_GetSoundSpeed.argtypes = (c_void_p, POINTER(c_int), )
dll.BVTPing_GetImage.restype = c_int
dll.BVTPing_GetImage.argtypes = (c_void_p, POINTER(c_void_p), )
dll.BVTPing_GetImageXY.restype = c_int
dll.BVTPing_GetImageXY.argtypes = (c_void_p, POINTER(c_void_p), )
dll.BVTPing_GetImageRTheta.restype = c_int
dll.BVTPing_GetImageRTheta.argtypes = (c_void_p, POINTER(c_void_p), )
dll.BVTPing_GetMaxReturnSignal.restype = c_int
dll.BVTPing_GetMaxReturnSignal.argtypes = (c_void_p, POINTER(c_float), )
dll.BVTPing_GetNavDataCopy.restype = c_int
dll.BVTPing_GetNavDataCopy.argtypes = (c_void_p, POINTER(c_void_p), )
dll.BVTPing_HasNavData.restype = c_int
dll.BVTPing_HasNavData.argtypes = (c_void_p, POINTER(c_int), )
dll.BVTPing_SetNavData.restype = c_int
dll.BVTPing_SetNavData.argtypes = (c_void_p, c_void_p, )
dll.BVTPing_GetVideoFrame.restype = c_int
dll.BVTPing_GetVideoFrame.argtypes = (c_void_p, POINTER(POINTER(c_char)), POINTER(c_int), POINTER(c_int), POINTER(c_int), POINTER(c_int), )
dll.BVTPing_SetVideoFrameJPEG.restype = c_int
dll.BVTPing_SetVideoFrameJPEG.argtypes = (c_void_p, c_void_p, c_int, c_int, c_int, )
dll.BVTPing_GetPositionerOrientationCopy.restype = c_int
dll.BVTPing_GetPositionerOrientationCopy.argtypes = (c_void_p, POINTER(c_void_p), )
dll.BVTPing_SetPositionerRotations.restype = c_int
dll.BVTPing_SetPositionerRotations.argtypes = (c_void_p, c_double, c_double, c_double, )
dll.BVTPing_GetPositionerRotations.restype = c_int
dll.BVTPing_GetPositionerRotations.argtypes = (c_void_p, POINTER(c_double), POINTER(c_double), POINTER(c_double), )
dll.BVTPing_HasSignal.restype = c_int
dll.BVTPing_HasSignal.argtypes = (c_void_p, POINTER(c_int), )
dll.BVTPing_HasStoredRangeProfile.restype = c_int
dll.BVTPing_HasStoredRangeProfile.argtypes = (c_void_p, POINTER(c_int), )

