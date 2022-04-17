import os
import re
import shutil
from glob import glob


base_dirpath = os.path.dirname(__file__)
old_version_archive_dirpath = 'old_version'
creo_file_extensions = ('prt', 'asm')


def init():
    os.makedirs(old_version_archive_dirpath, exist_ok=True)


def check_creo_version_string(filename):
    for extension in creo_file_extensions:
        if re.search(rf'{extension}\.\d+', filename):
            return True
    else:
        return False


def get_creo_file_name():
    file_list = []
    for filename in glob('*'):
        if check_creo_version_string(filename):
            creoname = os.path.basename(filename).split('.')[0]
            if creoname not in file_list:
                file_list.append(creoname)
    return file_list


def move_old_version(archive_path=old_version_archive_dirpath):
    creofile_list = get_creo_file_name()
    for creofile in creofile_list:
        version_list = []
        for filename in glob('*'):
            if creofile == os.path.basename(filename).split('.')[0]:
                if check_creo_version_string(filename):
                    version_list.append(filename)
        version_list = sorted(version_list, key=lambda x: int(x.split('.')[-1]))
        while len(version_list) > 1:
            shutil.move(version_list.pop(0), archive_path)


def archive_old_version(name='archive', archive_path=old_version_archive_dirpath):
    shutil.make_archive(name, 'zip', archive_path)
    shutil.rmtree(archive_path)


def run():
    move_old_version(old_version_archive_dirpath)
    archive_old_version(archive_path=old_version_archive_dirpath)


if __name__ == '__main__':
    init()
    run()
