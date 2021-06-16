#include <mathwidgets.h>

N::MatricesLists:: MatricesLists ( QWidget * parent , Plan * p )
                 : TreeDock      (           parent ,        p )
                 , SceneManager  (                           p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::MatricesLists::~MatricesLists(void)
{
}

QSize N::MatricesLists::sizeHint(void) const
{
  return SizeSuggestion ( QSize ( 640 , 480 ) ) ;
}

void N::MatricesLists::Configure(void)
{
  setAccessibleName       ( "N::MatricesLists"  ) ;
  setObjectName           ( "N::MatricesLists"  ) ;
  /////////////////////////////////////////////////
  NewTreeWidgetItem       ( head                ) ;
  head->setText           ( 0 ,tr("Name"      ) ) ;
  head->setText           ( 1 ,tr("Identifier") ) ;
  head->setText           ( 2 ,tr("Type"      ) ) ;
  head->setText           ( 3 ,tr("Rows"      ) ) ;
  head->setText           ( 4 ,tr("Columns"   ) ) ;
  setRootIsDecorated      ( false               ) ;
  setAlternatingRowColors ( true                ) ;
  setColumnCount          ( 5                   ) ;
  assignHeaderItems       ( head                ) ;
  setWindowTitle          ( tr("Matrices")      ) ;
  MountClicked            ( 2                   ) ;
  plan -> setFont         ( this                ) ;
  /////////////////////////////////////////////////
  Types [ Cpp::Void      ] = "void"               ;
  Types [ Cpp::Char      ] = "char"               ;
  Types [ Cpp::Byte      ] = "unsigned char"      ;
  Types [ Cpp::Short     ] = "short"              ;
  Types [ Cpp::UShort    ] = "unsignged short"    ;
  Types [ Cpp::Integer   ] = "int"                ;
  Types [ Cpp::UInt      ] = "unsigned int"       ;
  Types [ Cpp::LongLong  ] = "long long"          ;
  Types [ Cpp::ULongLong ] = "unsigned long long" ;
  Types [ Cpp::Float     ] = "float"              ;
  Types [ Cpp::Double    ] = "double"             ;
  Types [ Cpp::LDouble   ] = "long double"        ;
}

bool N::MatricesLists::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false           ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Refresh      , startup     ( ) ) ;
  LinkAction        ( Insert       , Insert      ( ) ) ;
  return true                                          ;
}

void N::MatricesLists::run(int T,ThreadData * d)
{
  nDropOut         ( ! IsContinue ( d ) ) ;
  switch           ( T                  ) {
    case 10001                            :
      startLoading (                    ) ;
      Load         ( d                  ) ;
      stopLoading  (                    ) ;
    break                                 ;
  }                                       ;
}

bool N::MatricesLists::Load(ThreadData * d)
{
  nKickOut     ( ! IsContinue ( d ) , false            ) ;
  WaitClear    (                                       ) ;
  EnterSQL     ( SC , plan -> sql                      ) ;
    UUIDs   U = Matrices ( SC )                          ;
    SUID    u                                            ;
    QString Q                                            ;
    while ( IsContinue ( d ) && ( U . count ( ) > 0 ) )  {
      u = U . first ( )                                  ;
      U . takeFirst ( )                                  ;
      ////////////////////////////////////////////////////
      QString name                                       ;
      QString identifier                                 ;
      name = Name                                        (
               SC                                        ,
               u                                         ,
               vLanguageId                             ) ;
      Q = SC . sql . SelectFrom                          (
            "name,type,rows,columns"                     ,
            PlanTable(Matrices)                          ,
            SC . WhereUuid ( u )                       ) ;
      if ( SC . Fetch ( Q ) )                            {
        identifier  = SC . String ( 0 )                  ;
        int type    = SC . Int    ( 1 )                  ;
        int rows    = SC . Int    ( 2 )                  ;
        int columns = SC . Int    ( 3 )                  ;
        NewTreeWidgetItem ( item )                       ;
        item -> setData ( 0 , Qt::UserRole,u           ) ;
        item -> setText ( 0 , name                     ) ;
        item -> setText ( 1 , identifier               ) ;
        item -> setText ( 2 , Types [ type ]           ) ;
        item -> setText ( 3 , QString::number(rows)    ) ;
        item -> setText ( 4 , QString::number(columns) ) ;
        addTopLevelItem ( item                         ) ;
      }                                                  ;
    }                                                    ;
  LeaveSQL     ( SC , plan -> sql                      ) ;
  emit AutoFit (                                       ) ;
  Alert        ( Done                                  ) ;
  return true                                            ;
}

bool N::MatricesLists::startup(void)
{
  start ( 10001 ) ;
  return true     ;
}

void N::MatricesLists::Insert(void)
{
  emit Add ( this ) ;
}

void N::MatricesLists::Updated(SUID uuid)
{
  startup ( ) ;
}

void N::MatricesLists::doubleClicked(QTreeWidgetItem * item,int column)
{
  emit Edit ( this , nTreeUuid ( item , 0 ) ) ;
}

bool N::MatricesLists::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                           ;
  QTreeWidgetItem * it = currentItem ( )              ;
  QAction         * aa                                ;
  /////////////////////////////////////////////////////
  if ( NotNull ( it ) ) mm . add ( 101 , tr("Edit") ) ;
  mm . add ( 102 , tr("Add"    ) )                    ;
  mm . add ( 201 , tr("Reflush") )                    ;
  /////////////////////////////////////////////////////
  DockingMenu    ( mm   )                             ;
  mm . setFont   ( plan )                             ;
  aa = mm . exec (      )                             ;
  /////////////////////////////////////////////////////
  if ( IsNull     (      aa ) ) return true           ;
  if ( RunDocking ( mm , aa ) ) return true           ;
  /////////////////////////////////////////////////////
  switch ( mm [ aa ] )                                {
    case 101                                          :
      doubleClicked ( it , 0 )                        ;
    break                                             ;
    case 102                                          :
      Insert        (        )                        ;
    break                                             ;
    case 201                                          :
      startup       (        )                        ;
    break                                             ;
    default                                           :
    break                                             ;
  }                                                   ;
  /////////////////////////////////////////////////////
  return true                                         ;
}
