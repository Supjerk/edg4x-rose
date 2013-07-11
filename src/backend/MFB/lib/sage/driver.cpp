
#include "MFB/Sage/driver.hpp"

#include "sage3basic.h"

#ifndef PATCHING_SAGE_BUILDER_ISSUES
#  define PATCHING_SAGE_BUILDER_ISSUES 1
#endif

namespace MultiFileBuilder {

Driver<Sage>::Driver(SgProject * project_) :
  file_id_counter(1), // 0 is reserved for no file
  id_to_name_map(),
  file_pair_map(),
  file_pair_name_map(),
  standalone_source_file_map(),
  standalone_source_file_name_map(),
  file_to_id_map(),
  symbol_to_file_id_map(),
  parent_map(),
  project(project_ == NULL ? new SgProject::SgProject() : project_)
{ 
  if (project_ == NULL) {
    std::vector<std::string> arglist;
      arglist.push_back("c++");
      arglist.push_back("-c");
    project->set_originalCommandLineArgumentList (arglist);
  }

  if (!CommandlineProcessing::isCppFileNameSuffix("hpp"))
    CommandlineProcessing::extraCppSourceFileSuffixes.push_back("hpp");
}

unsigned long Driver<Sage>::createPairOfFiles(const std::string & name) {
  std::map<std::string, unsigned long>::iterator it_pair_file_name = file_pair_name_map.find(name);
  std::map<std::string, unsigned long>::iterator it_standalone_source_file_name = standalone_source_file_name_map.find(name);

  if (it_standalone_source_file_name != standalone_source_file_name_map.end()) {
    std::cerr << "{ERROR} [Driver::createPairOfFiles] Asked to build a file pair when a standalone source file exists." << std::endl;
    assert(false);
  }

  if (it_pair_file_name != file_pair_name_map.end()) {
    std::cerr << "{WARNING} [Driver::createPairOfFiles] Asked to build a file pair that already exists (return previously assigned ID)." << std::endl;
    return it_pair_file_name->second;
  }

  unsigned long id = file_id_counter++;

  std::pair<SgSourceFile *, SgSourceFile *> files(NULL, NULL);

  std::string filename;

  {
    filename = name + ".hpp";
    SgSourceFile * decl_file = isSgSourceFile(SageBuilder::buildFile(filename, filename, project));
    SageInterface::attachComment(decl_file, "/* File generated by Driver<Model>::createPairOfFiles(\"" + name + "\") */");
    files.first = decl_file;

    filename = name + ".cpp";
    SgSourceFile * defn_file = isSgSourceFile(SageBuilder::buildFile(filename, filename, project));
    SageInterface::attachComment(defn_file, "/* File generated by Driver<Model>::createPairOfFiles(\"" + name + "\") */");
    SageInterface::insertHeader(defn_file, name + ".hpp");
    files.second = defn_file;
  }

  id_to_name_map.insert(std::pair<unsigned long, std::string>(id, name));

  file_pair_map.insert(std::pair<unsigned long, std::pair<SgSourceFile *, SgSourceFile *> >(id, files));
  file_pair_name_map.insert(std::pair<std::string, unsigned long>(name, id));

  file_to_id_map.insert(std::pair<SgSourceFile *, unsigned long>(files.first,  id));
  file_to_id_map.insert(std::pair<SgSourceFile *, unsigned long>(files.second, id));

  // Create the set of accesible files from these files. Add this file pair.
  file_id_to_accessible_file_id_map.insert(std::pair<SgSourceFile *, std::set<unsigned long> >(files.first,  std::set<unsigned long>())).first->second.insert(id);
  file_id_to_accessible_file_id_map.insert(std::pair<SgSourceFile *, std::set<unsigned long> >(files.second, std::set<unsigned long>())).first->second.insert(id);

  return id;
}

unsigned long Driver<Sage>::createStandaloneSourceFile(const std::string & name, std::string suffix) {
  std::map<std::string, unsigned long>::iterator it_pair_file_name = file_pair_name_map.find(name);
  std::map<std::string, unsigned long>::iterator it_standalone_source_file_name = standalone_source_file_name_map.find(name);

  if (it_standalone_source_file_name != standalone_source_file_name_map.end()) {
    std::cerr << "{WARNING} [Driver::addStandaloneSourceFile] Asked to build a standalone source file that already exists (return previously assigned ID)." << std::endl;
    return it_standalone_source_file_name->second;
  }

  if (it_pair_file_name != file_pair_name_map.end()) {
    std::cerr << "{ERROR} [Driver::addStandaloneSourceFile] Asked to build a standalone source file when a file pair of same name exists." << std::endl;
    assert(false);
  }

  unsigned long id = file_id_counter++;

  std::string filename = name + "." + suffix;
  SgSourceFile * file = isSgSourceFile(SageBuilder::buildFile(filename, filename, project));
  SageInterface::attachComment(file, "/* File generated by Driver<Model>::createStandaloneSourceFile(\"" + name + "\") */");

  id_to_name_map.insert(std::pair<unsigned long, std::string>(id, name));

  standalone_source_file_map.insert(std::pair<unsigned long, SgSourceFile *>(id, file));
  standalone_source_file_name_map.insert(std::pair<std::string, unsigned long>(name, id));

  file_to_id_map.insert(std::pair<SgSourceFile *, unsigned long>(file, id));

  // Create the set of accesible files from this file. Add this file.
  file_id_to_accessible_file_id_map.insert(std::pair<SgSourceFile *, std::set<unsigned long> >(file, std::set<unsigned long>())).first->second.insert(id);

  return id;
}

unsigned long Driver<Sage>::addStandaloneSourceFile(SgSourceFile * source_file) {
  std::string name = source_file->getFileName();

  std::map<std::string, unsigned long>::iterator it_pair_file_name = file_pair_name_map.find(name);
  std::map<std::string, unsigned long>::iterator it_standalone_source_file_name = standalone_source_file_name_map.find(name);

  if (it_standalone_source_file_name != standalone_source_file_name_map.end()) {
    std::cerr << "{ERROR} [Driver::addStandaloneSourceFile] Asked to add a standalone source file (from existing SgSourceFile) that already exists." << std::endl;
    assert(false);
  }
  
  if (it_pair_file_name != file_pair_name_map.end()) {
    std::cerr << "{ERROR} [Driver::addStandaloneSourceFile] Asked to add a standalone source file (from existing SgSourceFile) when a file pair of same name exists." << std::endl;
    assert(false);
  }
  
  unsigned long id = file_id_counter++;

  id_to_name_map.insert(std::pair<unsigned long, std::string>(id, name));

  standalone_source_file_map.insert(std::pair<unsigned long, SgSourceFile *>(id, source_file));
  standalone_source_file_name_map.insert(std::pair<std::string, unsigned long>(name, id));

  file_to_id_map.insert(std::pair<SgSourceFile *, unsigned long>(source_file, id));

  // Create the set of accesible files from this file. Add this file.
  file_id_to_accessible_file_id_map.insert(std::pair<SgSourceFile *, std::set<unsigned long> >(source_file, std::set<unsigned long>())).first->second.insert(id);

  return id;
}

void Driver<Sage>::addIncludeDirectives(SgSourceFile * target_file, unsigned long to_be_included_file_id) {
  std::string to_be_included_file_name;

  std::map<unsigned long, std::pair<SgSourceFile *, SgSourceFile *> >::iterator it_file_pair = file_pair_map.find(to_be_included_file_id);
  assert(it_file_pair != file_pair_map.end()); // Need to be from a pair header and source, not a standalone source file

  std::map<unsigned long, std::string>::iterator it_name = id_to_name_map.find(to_be_included_file_id);
  assert(it_name != id_to_name_map.end());

  to_be_included_file_name = it_name->second + ".hpp";

  SageInterface::insertHeader(target_file, to_be_included_file_name);
}

}

