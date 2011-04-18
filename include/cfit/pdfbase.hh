#ifndef __PDFBASE_HH__
#define __PDFBASE_HH__

#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include <cfit/variable.hh>
#include <cfit/parameter.hh>
#include <cfit/pdfexception.hh>
#include <cfit/functors.hh>


class PdfBase
{
  friend class Pdf;

protected:
  std::map< std::string, Variable  > _vars;
  std::map< std::string, Parameter > _pars;

public:
  virtual ~PdfBase() {};

  // Setters.
  virtual void setVars( const std::vector< double >& vars ) throw( PdfException ) = 0;
  virtual void setPars( const std::vector< double >& pars ) throw( PdfException ) = 0;

  // Getters.
  // ALERTA: SI NO ES RETORNEN REFER�NCIES NO VA B� L'ASSIGNACI� AMB setVars I setPars.
//const std::map< std::string, Variable  >& getVariables()  const { return _vars;        }
  const std::map< std::string, Parameter >& getParameters() const { return _pars;        }
  const unsigned                            nVars()         const { return _vars.size(); }
  const unsigned                            nPars()         const { return _pars.size(); }
  const std::vector< std::string >          varNames()      const;

  // Before evaluating the pdf at all data points, cache anything common to
  //    all points (usually compute the norm).
  virtual void cache() = 0;

  virtual double evaluate()                                    const throw( PdfException ) = 0;
  virtual double evaluate( const std::vector< double >& vars ) const throw( PdfException ) = 0;
};

#endif