
#include "MFB/Sage/driver.hpp"

#include "sage3basic.h"

#ifndef PATCHING_SAGE_BUILDER_ISSUES
#  define PATCHING_SAGE_BUILDER_ISSUES 1
#endif

namespace MultiFileBuilder {

bool ignore(const std::string & name) {
  return name.find("__builtin") == 0;
}

bool ignore(SgScopeStatement * scope) {
  return isSgBasicBlock(scope);
}

template <>
void Driver<Sage>::loadSymbolsFromPair<SgDeclarationStatement>(unsigned long file_id, SgSourceFile * header_file, SgSourceFile * source_file) {
  loadSymbolsFromPair<SgNamespaceDeclarationStatement>(file_id, header_file, source_file);
  loadSymbolsFromPair<SgFunctionDeclaration>(file_id, header_file, source_file);
  loadSymbolsFromPair<SgClassDeclaration>(file_id, header_file, source_file);
  loadSymbolsFromPair<SgVariableDeclaration>(file_id, header_file, source_file);
  loadSymbolsFromPair<SgMemberFunctionDeclaration>(file_id, header_file, source_file);
}

Driver<Sage>::Driver(SgProject * project_) :
  project(project_),
  file_id_counter(1), // 0 is reserved
  file_pair_map(),
  standalone_source_file_map(),
  file_to_id_map(),
  p_symbol_to_file_id_map(),
  p_valid_symbols(),
  p_parent_map(),
  p_namespace_symbols(),
  p_function_symbols(),
  p_class_symbols(),
  p_variable_symbols(),
  p_member_function_symbols()
{ 
  assert(project != NULL);
/*
    std::vector<std::string> arglist;
      arglist.push_back("c++");
      arglist.push_back("-DSKIP_ROSE_BUILTIN_DECLARATIONS");
      arglist.push_back("-c");
    project->set_originalCommandLineArgumentList (arglist);
  }
*/
  if (!CommandlineProcessing::isCppFileNameSuffix("hpp"))
    CommandlineProcessing::extraCppSourceFileSuffixes.push_back("hpp");
}

unsigned long Driver<Sage>::createPairOfFiles(const std::string & name) {
  std::string filename;

  filename = name + ".hpp";
  SgSourceFile * header_file = isSgSourceFile(SageBuilder::buildFile(filename, filename, project));
  SageInterface::attachComment(header_file, "/* File generated by Driver<Model>::createPairOfFiles(\"" + name + "\") */");

  filename = name + ".cpp";
  SgSourceFile * source_file = isSgSourceFile(SageBuilder::buildFile(filename, filename, project));
  SageInterface::attachComment(source_file, "/* File generated by Driver<Model>::createPairOfFiles(\"" + name + "\") */");

  unsigned long id = addPairOfFiles(header_file, source_file);

  addIncludeDirectives(source_file, id);

  return id;
}

unsigned long Driver<Sage>::loadPairOfFiles(const std::string & name, const std::string & header_path, const std::string & source_path) {
  std::string filename;

  filename = name + ".hpp";
  SgSourceFile * header_file = isSgSourceFile(SageBuilder::buildFile(header_path + filename, filename, project)); // FIXME could extract the header from the source file...

  filename = name + ".cpp";
  SgSourceFile * source_file = isSgSourceFile(SageBuilder::buildFile(source_path + filename, filename, project));

  unsigned long id = addPairOfFiles(header_file, source_file);

  loadSymbolsFromPair<SgDeclarationStatement>(id, header_file, source_file);

  return id;
}

unsigned long Driver<Sage>::addPairOfFiles(SgSourceFile * header_file, SgSourceFile * source_file) {
  unsigned long id = file_id_counter++;

  file_pair_map.insert(std::pair<unsigned long, std::pair<SgSourceFile *, SgSourceFile *> >(id, std::pair<SgSourceFile *, SgSourceFile *>(header_file, source_file)));

  file_to_id_map.insert(std::pair<SgSourceFile *, unsigned long>(header_file, id));
  file_to_id_map.insert(std::pair<SgSourceFile *, unsigned long>(source_file, id));

  // Create the set of accesible files from these files. Add this file pair.
  file_id_to_accessible_file_id_map.insert(std::pair<SgSourceFile *, std::set<unsigned long> >(header_file,  std::set<unsigned long>())).first->second.insert(id);
  file_id_to_accessible_file_id_map.insert(std::pair<SgSourceFile *, std::set<unsigned long> >(source_file, std::set<unsigned long>())).first->second.insert(id);

  return id;
}

unsigned long Driver<Sage>::createStandaloneSourceFile(const std::string & name, std::string suffix) {
  std::string filename = name + "." + suffix;
  SgSourceFile * file = isSgSourceFile(SageBuilder::buildFile(filename, filename, project));
  SageInterface::attachComment(file, "/* File generated by Driver<Model>::createStandaloneSourceFile(\"" + name + "\") */");

  return addStandaloneSourceFile(file);
}

unsigned long Driver<Sage>::addStandaloneSourceFile(SgSourceFile * source_file) {
  unsigned long id = file_id_counter++;

  standalone_source_file_map.insert(std::pair<unsigned long, SgSourceFile *>(id, source_file));

  file_to_id_map.insert(std::pair<SgSourceFile *, unsigned long>(source_file, id));

  // Create the set of accesible files from this file. Add this file.
  file_id_to_accessible_file_id_map.insert(std::pair<SgSourceFile *, std::set<unsigned long> >(source_file, std::set<unsigned long>())).first->second.insert(id);

  return id;
}

void Driver<Sage>::addIncludeDirectives(SgSourceFile * target_file, unsigned long to_be_included_file_id) {
  std::string to_be_included_file_name;

  std::map<unsigned long, std::pair<SgSourceFile *, SgSourceFile *> >::iterator it_file_pair = file_pair_map.find(to_be_included_file_id);
  assert(it_file_pair != file_pair_map.end()); // Need to be from a pair header and source, not a standalone source file

  SgSourceFile * header_file = it_file_pair->second.first;
  assert(header_file != NULL);

  to_be_included_file_name = header_file->getFileName(); // FIXME path??

  SageInterface::insertHeader(target_file, to_be_included_file_name);
}

}
