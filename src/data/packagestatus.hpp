#pragma once

enum class PackageStatus: uint8_t
{
	/**
	* Unknown status
	 */
	Unknown = 0,

	/**
	* Package is up to date and recent
	*/
	UpToDate = 1,

	/**
	* There's a new version available
	*/
	Outdated = 2,

	/**
	* Contains a known security vulnerability
	*/
	Vulnerable = 3,

	/**
	* Hasn't been updated in a long time
	*/
	Unmaintained = 3,

	/**
	* Package is considered deprecated and no longer receives updates
	*/
	Deprecated = 4,
};
