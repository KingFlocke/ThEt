/*
 * VTKMoleculeWriterImplementation.h
 *
 * @Date: 25.08.2010
 * @Author: Wolfgang Eckhardt
 */

#ifndef VTKMOLECULEWRITERIMPLEMENTATION_H_
#define VTKMOLECULEWRITERIMPLEMENTATION_H_


#include <string>
#include <vector>

class Molecule;
class VTKFile_t;

/**
 * This class contains the actual functionality for writing molecules as
 * vtk unstructured grid data.
 * Molecules are written as the points of the unstructured grid, thus there are
 * no grid cells, as the molecules don't really form a grid.
 *
 * It acts as an adapter to the serialization classes generated by codesynthesis xsd.
 */
class VTKMoleculeWriterImplementation {

private:

	//! object representing the sequential vkt-file
	VTKFile_t* _vtkFile;

	//! object representing the parallel vkt-file
	VTKFile_t* _parallelVTKFile;

    //! Counts the number of molecules plotted.
	unsigned int _numMoleculesPlotted;

	//! the rank of the process
	int _rank;

	//! if all centers should be ploted separately
	bool _plotCenters;

	enum CenterType { Charge = 1, LJ = 2, Dipole = 3, Quadrupole = 4, Tersoff = 5 };

	/**
	 * plots one single center
	 */
	void plotCenter(Molecule& molecule, int centerID, CenterType centerType);

public:

	/**
	 * @param rank the MPI rank of the process
	 */
	VTKMoleculeWriterImplementation(int rank, bool plotCenters = false);
	virtual ~VTKMoleculeWriterImplementation();

	void initializeVTKFile();

	/**
	 * Plot a molecule.
	 *
	 * @param molecule the molecule to plot
	 */
	void plotMolecule(Molecule& molecule);
	void writeVTKFile(const std::string& fileName);

	/**
	 * Initialize the data structures to write a parallel vtk file (i.e. a meta
	 * file describing the data and structure of the file a single node creates).
	 *
	 * @param fileNames the names of the sequential files which are to be referenced
	 */
	void initializeParallelVTKFile(const std::vector<std::string>& fileNames);


	/**
	 * Write the xml-tree to a file on disc.
	 *
	 * @param fileName the name of the file written
	 */
	void writeParallelVTKFile(const std::string& fileName);

	bool isVTKFileInitialized();

	bool isParallelVTKFileInitialized();

	unsigned int getNumMoleculesPlotted();
};

#endif /* VTKMOLECULEWRITERIMPLEMENTATION_H_ */
