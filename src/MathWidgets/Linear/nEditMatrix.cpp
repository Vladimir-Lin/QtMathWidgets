#include <mathwidgets.h>
#include "ui_nEditMatrix.h"

N::EditMatrix:: EditMatrix   ( QWidget * parent , Plan * p    )
              : Widget       (           parent ,        p    )
              , Object       ( 0                , Types::None )
              , SceneManager (                           p    )
              , ui           ( new Ui::nEditMatrix            )
              , Editor       ( NULL                           )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::EditMatrix::~EditMatrix(void)
{
  delete ui ;
}

void N::EditMatrix::Configure(void)
{
  ui     -> setupUi          ( this        )                      ;
  Editor  = new MatrixEditor ( this , plan )                      ;
  Editor -> matrix = &matrix                                      ;
  /////////////////////////////////////////////////////////////////
  ui -> Type -> addItem ( "char"               , Cpp::Char      ) ;
  ui -> Type -> addItem ( "unsigned char"      , Cpp::Byte      ) ;
  ui -> Type -> addItem ( "short"              , Cpp::Short     ) ;
  ui -> Type -> addItem ( "unsignged short"    , Cpp::UShort    ) ;
  ui -> Type -> addItem ( "int"                , Cpp::Integer   ) ;
  ui -> Type -> addItem ( "unsigned int"       , Cpp::UInt      ) ;
  ui -> Type -> addItem ( "long long"          , Cpp::LongLong  ) ;
  ui -> Type -> addItem ( "unsigned long long" , Cpp::ULongLong ) ;
  ui -> Type -> addItem ( "float"              , Cpp::Float     ) ;
  ui -> Type -> addItem ( "double"             , Cpp::Double    ) ;
  ui -> Type -> addItem ( "long double"        , Cpp::LDouble   ) ;
  /////////////////////////////////////////////////////////////////
  *(ui->Type) <= Cpp::Double                                      ;
  /////////////////////////////////////////////////////////////////
  plan -> setFont ( this )                                        ;
}

void N::EditMatrix::resizeEvent(QResizeEvent * event)
{
  QWidget :: resizeEvent ( event ) ;
  Relocation             (       ) ;
}

void N::EditMatrix::showEvent(QShowEvent * event)
{
  QWidget :: showEvent ( event ) ;
  Relocation           (       ) ;
}

bool N::EditMatrix::startup(void)
{
  Editor -> startup                 (                      ) ;
  ui     -> Row    -> blockSignals  ( true                 ) ;
  ui     -> Column -> blockSignals  ( true                 ) ;
  ui     -> Type   -> blockSignals  ( true                 ) ;
  ui     -> Row    -> setValue      ( matrix . Rows    ( ) ) ;
  ui     -> Column -> setValue      ( matrix . Columns ( ) ) ;
  *( ui  -> Type ) <= matrix . Type (                      ) ;
  ui     -> Row    -> blockSignals  ( false                ) ;
  ui     -> Column -> blockSignals  ( false                ) ;
  ui     -> Type   -> blockSignals  ( false                ) ;
  return true                                                ;
}

bool N::EditMatrix::load(SUID uu)
{
  setObjectUuid  ( uu                            ) ;
  EnterSQL       ( SC , plan -> sql              ) ;
    QString name                                   ;
    QString Q                                      ;
    name = Name  ( SC                              ,
                   ObjectUuid ( )                  ,
                   vLanguageId                   ) ;
    matrix = getMatrix ( SC , ObjectUuid ( )     ) ;
    ui    -> Name       -> blockSignals ( true   ) ;
    ui    -> Identifier -> blockSignals ( true   ) ;
    ui    -> Name       -> setText      ( name   ) ;
    Q      = SC . sql . SelectFrom                 (
               "name"                              ,
               PlanTable(Matrices)                 ,
               SC . WhereUuid ( ObjectUuid ( ) ) ) ;
    if ( SC . Fetch ( Q ) )                        {
      QString identifier = SC . String ( 0 )       ;
      ui -> Identifier -> setText ( identifier )   ;
    }                                              ;
    ui -> Name       -> blockSignals ( false     ) ;
    ui -> Identifier -> blockSignals ( false     ) ;
  LeaveSQL       ( SC , plan -> sql              ) ;
  return startup (                               ) ;
}

void N::EditMatrix::Relocation(void)
{
  QRect gi = ui->Identifier->geometry()                                        ;
  Editor->move(gi.left(),gi.bottom())                                          ;
  Editor->resize(width(),height()-gi.bottom())                                 ;
  //////////////////////////////////////////////////////////////////////////////
  ui->Name      ->resize(width()-ui->Name      ->x(),ui->Name      ->height()) ;
  ui->Type      ->resize(width()-ui->Type      ->x(),ui->Type      ->height()) ;
  ui->Identifier->resize(width()-ui->Identifier->x(),ui->Identifier->height()) ;
}

void N::EditMatrix::positionChanged(int)
{
  Editor -> setHeader ( ui -> Position -> isChecked ( ) ) ;
}

void N::EditMatrix::nameChanged(QString text)
{
  canSave ( ) ;
}

void N::EditMatrix::canSave(void)
{
  QString name = ui -> Name -> text ( ) ;
  bool save = ( name . length ( ) > 0 ) ;
  ui -> Save -> setEnabled ( save )     ;
}

void N::EditMatrix::dimChanged(int)
{
  int row    = ui -> Row    -> value ( )                       ;
  int column = ui -> Column -> value ( )                       ;
  int type   = ui -> Type   -> itemData ( ui -> Type -> currentIndex ( ) ) . toInt ( ) ;
  //////////////////////////////////////////////////////////////
  matrix  . set      ( (Cpp::ValueTypes) type , row , column ) ;
  matrix  . zerofill (                                       ) ;
  Editor -> startup  (                                       ) ;
  positionChanged    ( 0                                     ) ;
}

void N::EditMatrix::Save(void)
{
  QString Name       = ui -> Name       -> text ( ) ;
  QString Identifier = ui -> Identifier -> text ( ) ;
  if ( Name . length ( ) <= 0 ) return              ;
  ///////////////////////////////////////////////////
  EnterSQL                 ( SC , plan -> sql     ) ;
    uuid = updateMatrix                             (
             SC                                     ,
             ObjectUuid ( )                         ,
             Name                                   ,
             Identifier                             ,
             matrix                               ) ;
  LeaveSQL                 ( SC , plan -> sql     ) ;
  ///////////////////////////////////////////////////
  setWindowTitle           ( Name                 ) ;
  ui -> Save -> setEnabled ( false                ) ;
  emit Updated             ( ObjectUuid ( )       ) ;
  Alert                    ( Done                 ) ;
}
