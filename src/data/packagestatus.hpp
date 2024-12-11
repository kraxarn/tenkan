#pragma once

enum class PackageStatus: uint8_t
{
	/**
	* Unknown status
	 */
	Unknown = 0,

	/**
	 * Updating status
	 */
	Updating = 1,

	/**
	* Package is up to date and recent
	*/
	UpToDate = 2,

	/**
	* There's a new version available
	*/
	Outdated = 3,

	/**
	* Contains a known security vulnerability
	*/
	Vulnerable = 4,

	/**
	* Hasn't been updated in a long time
	*/
	Unmaintained = 5,

	/**
	* Package is considered deprecated and no longer receives updates
	*/
	Deprecated = 6,
};
