/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Delete.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaidi <hsaidi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/27 11:30:30 by hsaidi            #+#    #+#             */
/*   Updated: 2023/06/26 03:00:59 by hsaidi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Delete.hpp"

Delete::Delete(MainClient *main_client, vector<ConfigLocationParser *> config_location_parser) {
	this->main_client			 = main_client;
	this->config_location_parser = config_location_parser;
}

Delete::~Delete() {}

void Delete::deleted() {
	std::string file_name = this->main_client->get_new_url();

	if (!path_exists(file_name))
		throw Error::NotFound404();

	if (delete_path(file_name))
		throw Accurate::NoContent204();
}

bool Delete::path_exists(const std::string &path) {
	struct stat path_stat;
	return (stat(path.c_str(), &path_stat) == 0);
}

bool Delete::delete_path(const std::string &path) {
	bool success = false;

	// Check if the path exists
	if (access(path.c_str(), F_OK) == 0) {
		if (access(path.c_str(), W_OK) != 0)
			throw Error::Forbidden403();
		struct stat path_stat;
		if (stat(path.c_str(), &path_stat) == 0) {
			if (S_ISDIR(path_stat.st_mode)) {
				// Delete directory recursively
				success = delete_directory(path);
			} else if (S_ISREG(path_stat.st_mode)) {
				// Delete file
				if (remove(path.c_str()) == 0)
					success = true;
			}
		}
		if (!success)
			throw Error::Forbidden403();
	}
	return success;
}

bool Delete::delete_directory(const std::string &path) {
	DIR *dir = opendir(path.c_str());
	if (dir == NULL)
		return false;

	bool		   success = true;
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;

		std::string entry_path = path + "/" + entry->d_name;

		if (entry->d_type == DT_DIR) {
			if (!delete_directory(entry_path))
				success = false;
		} else if (entry->d_type == DT_REG) {
			if (remove(entry_path.c_str()) != 0)
				success = false;
		}
	}
	closedir(dir);

	if (success && remove(path.c_str()) == 0)
		return true;

	return false;
}