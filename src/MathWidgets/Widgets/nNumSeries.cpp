#include <mathwidgets.h>

N::NumSeries:: NumSeries   ( QWidget * parent , Plan * p )
             : TreeWidget  (           parent ,        p )
             , Object      ( 0 , Types::None             )
             , Name        ( ""                          )
             , ValueTypeId ( 0                           )
             , Precision   ( 8                           )
             , Radix       ( 10                          )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::NumSeries::~NumSeries(void)
{
}

bool N::NumSeries::FocusIn(void)
{
  AssignAction ( Label   , windowTitle ( ) ) ;
  LinkAction   ( Refresh , startup     ( ) ) ;
  LinkAction   ( Paste   , Paste       ( ) ) ;
  LinkAction   ( Save    , Save        ( ) ) ;
  return true                                ;
}

void N::NumSeries::Configure(void)
{
  NewTreeWidgetItem            ( head                   ) ;
  head -> setText              ( 0 , tr("ID"   )        ) ;
  head -> setText              ( 1 , tr("Value")        ) ;
  head -> setText              ( 2 , ""                 ) ;
  for (int i=0;i<2;i++)                                   {
    head -> setTextAlignment   ( i , Qt::AlignLeft        |
                                     Qt::AlignVCenter   ) ;
    setAlignment( i , Qt::AlignRight | Qt::AlignVCenter ) ;
  }                                                       ;
  setWindowTitle               ( tr("Numerical series") ) ;
  setDragDropMode              ( NoDragDrop             ) ;
  setRootIsDecorated           ( false                  ) ;
  setAlternatingRowColors      ( true                   ) ;
  setSelectionMode             ( ExtendedSelection      ) ;
  setColumnCount               ( 3                      ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded  ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded  ) ;
  setFont                      ( head , Fonts::ListView ) ;
  setHeaderItem                ( head                   ) ;
  plan -> setFont              ( this                   ) ;
  MountClicked                 ( 2                      ) ;
}

void N::NumSeries::run(int T,ThreadData * d)
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

bool N::NumSeries::Load(ThreadData * d)
{
  nKickOut ( ! IsContinue ( d ) , false ) ;
  return true                             ;
}

bool N::NumSeries::startup(void)
{
  Body . clear ( )                    ;
  EnterSQL       ( SC , plan->sql )   ;
    QString Q                         ;
    QString name                      ;
    int     size     = 0              ;
    int     checksum = 0              ;
    int     chksum   = 0              ;
    bool    correct  = true           ;
    name = SC.getName                 (
           PlanTable(Names)           ,
           "uuid"                     ,
           vLanguageId                ,
           ObjectUuid()             ) ;
    if (name.length()>0)              {
      setWindowTitle(name)            ;
    }                                 ;
    Q = SC.sql.SelectFrom             (
          "type,size,checksum,value"  ,
          PlanTable(Arrays)           ,
          SC.WhereUuid(ObjectUuid())) ;
    if (SC.Fetch(Q))                  {
      ValueTypeId = SC.Int      (0)   ;
      size        = SC.Int      (1)   ;
      checksum    = SC.Int      (2)   ;
      Body        = SC.ByteArray(3)   ;
      if (Body.size()>0)              {
        chksum = qChecksum            (
          (const char *)Body.data()   ,
          Body.size()               ) ;
        if (chksum!=checksum)         {
          correct = false             ;
        }                             ;
        if (size!=Body.size())        {
          correct = false             ;
        }                             ;
        if (!correct) Body.clear()    ;
      }                               ;
    }                                 ;
  LeaveSQL       ( SC , plan->sql )   ;
  ListItems      (                )   ;
  return true                         ;
}

void N::NumSeries::ListItems(void)
{
  clear (      )                                 ;
  int dsize = N::Cpp::SizeOf((N::Cpp::ValueTypes)ValueTypeId) ;
  nDropOut ( dsize <= 0  )                       ;
  int lsize = Body.size() / dsize                ;
  nDropOut ( lsize <= 0  )                       ;
  QTreeWidgetItem * head                         ;
  head = headerItem()                            ;
  #define LT(TYPE)                               \
    if (lsize>0)                               { \
      TYPE * d = (TYPE *)Body.data()           ; \
      nFullLoop ( i , lsize )                  { \
        NewTreeWidgetItem(item)                ; \
        item->setText(0,QString::number(i+1 )) ; \
        item->setText(1,QString::number(d[i])) ; \
        setAlignments   ( item )               ; \
        addTopLevelItem ( item )               ; \
      }                                        ; \
    }
  switch (ValueTypeId)                           {
    case N::Cpp::Short                    :
      head->setText(1,"short")                   ;
      LT(short)                                  ;
    break                                        ;
    case N::Cpp::UShort                   :
      head->setText(1,"unsigned short")          ;
      LT(unsigned short)                         ;
    break                                        ;
    case N::Cpp::Integer                  :
      head->setText(1,"int")                     ;
      LT(int)                                    ;
    break                                        ;
    case N::Cpp::UInt                     :
      head->setText(1,"unsigned int")            ;
      LT(unsigned int)                           ;
    break                                        ;
    case N::Cpp::LongLong                 :
      head->setText(1,"long long")               ;
      LT(TUID)                                   ;
    break                                        ;
    case N::Cpp::ULongLong                :
      head->setText(1,"unsigned long long")      ;
      LT(SUID)                                   ;
    break                                        ;
    case N::Cpp::Float                    :
      head->setText(1,"float")                   ;
      if (lsize>0)                               {
        float * d                                ;
        d = (float *)Body.data()                 ;
        nFullLoop ( i , lsize )                  {
          float v = (float)d[i]                  ;
          QString V                              ;
          V = QString::number(v,'f',Precision  ) ;
          NewTreeWidgetItem(item)                ;
          item->setText(0,QString::number(i+1 )) ;
          item->setText(1,V                    ) ;
          setAlignments   ( item )               ;
          addTopLevelItem ( item )               ;
        }                                        ;
      }                                          ;
    break                                        ;
    case N::Cpp::Double                   :
      head->setText(1,"double")                  ;
      if (lsize>0)                               {
        double * d                               ;
        d = (double *)Body.data()                ;
        nFullLoop ( i , lsize )                  {
          double v = (double)d[i]                ;
          QString V                              ;
          V = QString::number(v,'f',Precision  ) ;
          NewTreeWidgetItem(item)                ;
          item->setText(0,QString::number(i+1 )) ;
          item->setText(1,V                    ) ;
          setAlignments   ( item )               ;
          addTopLevelItem ( item )               ;
        }                                        ;
      }                                          ;
    break                                        ;
    case N::Cpp::LDouble                  :
      head->setText(1,"long double")             ;
      if (lsize>0)                               {
        long double * d                          ;
        d = (long double *)Body.data()           ;
        nFullLoop ( i , lsize )                  {
          double v = (double)d[i]                ;
          QString V                              ;
          V = QString::number(v,'f',Precision  ) ;
          NewTreeWidgetItem(item)                ;
          item->setText(0,QString::number(i+1 )) ;
          item->setText(1,V                    ) ;
          setAlignments   ( item )               ;
          addTopLevelItem ( item )               ;
        }                                        ;
      }                                          ;
    break                                        ;
  }                                              ;
  #undef  LT
  SuitableColumns ( )                            ;
  Alert ( Done )                                 ;
}

void N::NumSeries::Paste(QString & text)
{
  int dsize = N::Cpp::SizeOf((N::Cpp::ValueTypes)ValueTypeId) ;
  nDropOut ( dsize <= 0  )                           ;
  int lsize = Body.size() / dsize                    ;
  nDropOut ( lsize <= 0  )                           ;
  QTreeWidgetItem * item = currentItem()             ;
  int n = 0                                          ;
  if (NotNull(item))                                 {
    n = item->text(0).toInt() - 1                    ;
  }                                                  ;
  QString     t = text                               ;
  QStringList r                                      ;
  QStringList v                                      ;
  t = t.replace("{","")                              ;
  t = t.replace("}","")                              ;
  t = t.replace(";","")                              ;
  t = t.replace("=","")                              ;
  t = t.replace(",","\n")                            ;
  r = t.split('\n')                                  ;
  nFullLoop ( i , r.count() )                        {
    t = r[i]                                         ;
    t = t.replace(" ","")                            ;
    if (t.length()>0) v << t                         ;
  }                                                  ;
  #define LT(DTYPE,ATYPE,CFUNC)                      \
    if (lsize>0 && v.count()>0)                    { \
      DTYPE * d = (DTYPE *)Body.data()             ; \
      for (int i=0;n<lsize && i<v.count();i++,n++) { \
        d[n] = (ATYPE)v[i].CFUNC                   ; \
      }                                            ; \
    }
  switch (ValueTypeId)                               {
    case N::Cpp::Short                        :
      LT(short,short,toShort())                      ;
    break                                            ;
    case N::Cpp::UShort                       :
      LT(unsigned short,unsigned short,toUShort())   ;
    break                                            ;
    case N::Cpp::Integer                      :
      LT(int,int,toInt())                            ;
    break                                            ;
    case N::Cpp::UInt                         :
      LT(unsigned int,unsigned int,toUInt())         ;
    break                                            ;
    case N::Cpp::LongLong                     :
      LT(TUID,TUID,toLongLong ())                    ;
    break                                            ;
    case N::Cpp::ULongLong                    :
      LT(SUID,SUID,toULongLong ())                   ;
    break                                            ;
    case N::Cpp::Float                        :
      LT(float,float,toFloat())                      ;
    break                                            ;
    case N::Cpp::Double                       :
      LT(double,double,toDouble())                   ;
    break                                            ;
    case N::Cpp::LDouble                      :
      LT(long double,double,toDouble())              ;
    break                                            ;
  }                                                  ;
  #undef  LT
  ListItems ( )                                      ;
}

void N::NumSeries::Paste(void)
{
  QString text = nClipboardText ;
  if (text.length()<=0) return  ;
  Paste ( text )                ;
}

void N::NumSeries::Save(void)
{
  nDropOut ( Body.size() <= 0 ) ;
  nDropOut ( ObjectUuid()<= 0 ) ;
  EnterSQL ( SC , plan->sql   ) ;
    QString Q                   ;
    int     chksum              ;
    int     size                ;
    size = Body.size()          ;
    chksum = qChecksum          (
      (const char *)Body.data() ,
      size                    ) ;
    Q = SC.sql.Update(
          PlanTable(Arrays)     ,
          "where uuid = :UUID"  ,
          3                     ,
          "size"                ,
          "checksum"            ,
          "value"             ) ;
    SC.Prepare(Q              ) ;
    SC.Bind("uuid"    ,ObjectUuid()   ) ;
    SC.Bind("size"    ,size   ) ;
    SC.Bind("checksum",chksum ) ;
    SC.Bind("value"   ,Body   ) ;
    SC.Exec(                  ) ;
  LeaveSQL ( SC , plan->sql   ) ;
  Alert    ( Done             ) ;
}

void N::NumSeries::Resize(void)
{
  int dsize = N::Cpp::SizeOf((N::Cpp::ValueTypes)ValueTypeId) ;
  nDropOut ( dsize <= 0  )           ;
  int lsize = Body.size() / dsize    ;
  bool okay = false                  ;
  int  r    = 0                      ;
  r = QInputDialog::getInt           (
        this                         ,
        tr("Maximum items")          ,
        tr("Limit"        )          ,
        lsize                        ,
        1                            ,
        10000000                     ,
        1                            ,
        &okay                      ) ;
  if (okay) lsize = r                ;
       else return                   ;
  lsize *= dsize                     ;
  Body.resize(lsize)                 ;
  ListItems()                        ;
}

void N::NumSeries::ChangeType(void)
{
}

void N::NumSeries::setRadix(void)
{
}

void N::NumSeries::setPrecision(void)
{
  bool okay = false                    ;
  int  r    = 0                        ;
  r = QInputDialog::getInt             (
        this                           ,
        tr("Floating point precision") ,
        tr("Digits"                  ) ,
        Precision                      ,
        0                              ,
        100                            ,
        1                              ,
        &okay                        ) ;
  if (okay) Precision = r              ;
       else return                     ;
  ListItems()                          ;
}

void N::NumSeries::toCpp(void)
{
  BlobToCpp * BTC =  new BlobToCpp ( this , plan )   ;
  QString ht = BTC->windowTitle()                    ;
  ht = tr("%1 %2").arg(windowTitle()).arg(ht)        ;
  BTC->setWindowTitle(ht)                            ;
  if ( BTC -> setData ( &Body , ValueTypeId ) )      {
    if ( BTC->exec() != QDialog::Accepted )          {
      Alert ( Error )                                ;
    } else                                           {
      if ( BTC->Export ( ) )                         {
        Alert ( Done  )                              ;
      } else                                         {
        Alert ( Error )                              ;
      }                                              ;
    }                                                ;
  } else                                             {
    Alert ( Error )                                  ;
  }                                                  ;
  BTC->deleteLater()                                 ;
}

bool N::NumSeries::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )        ;
  QMenu      * me                  ;
  QAction    * aa                  ;
  mm.add(101,tr("Save"     ))      ;
  mm.add(102,tr("Export C/C++"))   ;
  me = mm.addMenu(tr("Edit"))      ;
  mm.add(me,201,tr("Resize"))      ;
  mm.add(me,202,tr("Type"  ))      ;
  if (ValueTypeId>=N::Cpp::Float && ValueTypeId<=N::Cpp::LDouble) {
    mm.add(me,203,tr("Precision")) ;
  }                                ;
  mm.add(me,204,tr("Radix"))       ;
  mm.setFont(plan)                 ;
  aa = mm.exec()                   ;
  if (IsNull(aa)) return true      ;
  switch (mm[aa])                  {
    case 101                       :
      Save         ( )             ;
    break                          ;
    case 102                       :
      toCpp        ( )             ;
    break                          ;
    case 201                       :
      Resize       ( )             ;
    break                          ;
    case 202                       :
      ChangeType   ( )             ;
    break                          ;
    case 203                       :
      setPrecision ( )             ;
    break                          ;
    case 204                       :
      setRadix     ( )             ;
    break                          ;
  }                                ;
  return true                      ;
}

void N::NumSeries::doubleClicked(QTreeWidgetItem * item,int column)
{
  if (column!=1) return         ;
  setLineEdit                   (
    item,1                      ,
    SIGNAL(editingFinished())   ,
    SLOT  (nameFinished   ()) ) ;
}

void N::NumSeries::nameFinished(void)
{
  QLineEdit * line = Casting(QLineEdit,ItemWidget)     ;
  nDropOut ( IsNull(line       ) )                     ;
  nDropOut ( IsNull(ItemEditing) )                     ;
  QString name = line->text()                          ;
  int     i    = ItemEditing->text(0).toInt()-1        ;
  switch (ValueTypeId)                                 {
    case N::Cpp::Short                                 :
      if (name.length()>0)                             {
        bool    okay = true                            ;
        short * d = (short *)Body.data()               ;
        short v = name.toShort(&okay)                  ;
        if (okay)                                      {
          d[i] = v                                     ;
          name = QString::number(v)                    ;
          ItemEditing->setText(1,name)                 ;
        }                                              ;
      }                                                ;
    break                                              ;
    case N::Cpp::UShort                                :
      if (name.length()>0)                             {
        bool    okay = true                            ;
        ushort * d = (ushort *)Body.data()             ;
        ushort   v = name.toUShort(&okay)              ;
        if (okay)                                      {
          d[i] = v                                     ;
          name = QString::number(v)                    ;
          ItemEditing->setText(1,name)                 ;
        }                                              ;
      }                                                ;
    break                                              ;
    case N::Cpp::Integer                               :
      if (name.length()>0)                             {
        bool    okay = true                            ;
        int * d = (int *)Body.data()                   ;
        int   v = name.toInt(&okay)                    ;
        if (okay)                                      {
          d[i] = v                                     ;
          name = QString::number(v)                    ;
          ItemEditing->setText(1,name)                 ;
        }                                              ;
      }                                                ;
    break                                              ;
    case N::Cpp::UInt                                  :
      if (name.length()>0)                             {
        bool    okay = true                            ;
        unsigned int * d = (unsigned int *)Body.data() ;
        unsigned int   v = name.toUInt(&okay)          ;
        if (okay)                                      {
          d[i] = v                                     ;
          name = QString::number(v)                    ;
          ItemEditing->setText(1,name)                 ;
        }                                              ;
      }                                                ;
    break                                              ;
    case N::Cpp::LongLong                              :
      if (name.length()>0)                             {
        bool    okay = true                            ;
        TUID * d = (TUID *)Body.data()                 ;
        TUID   v = name.toLongLong(&okay)              ;
        if (okay)                                      {
          d[i] = v                                     ;
          name = QString::number(v)                    ;
          ItemEditing->setText(1,name)                 ;
        }                                              ;
      }                                                ;
    break                                              ;
    case N::Cpp::ULongLong                             :
      if (name.length()>0)                             {
        bool    okay = true                            ;
        SUID * d = (SUID *)Body.data()                 ;
        SUID   v = name.toULongLong(&okay)             ;
        if (okay)                                      {
          d[i] = v                                     ;
          name = QString::number(v)                    ;
          ItemEditing->setText(1,name)                 ;
        }                                              ;
      }                                                ;
    break                                              ;
    case N::Cpp::Float                                 :
      if (name.length()>0)                             {
        bool    okay = true                            ;
        float * d = (float *)Body.data()               ;
        float   v = name.toFloat(&okay)                ;
        if (okay)                                      {
          d[i] = v                                     ;
          name = QString::number(v,'f',Precision)      ;
          ItemEditing->setText(1,name)                 ;
        }                                              ;
      }                                                ;
    break                                              ;
    case N::Cpp::Double                                :
      if (name.length()>0)                             {
        bool    okay = true                            ;
        double * d = (double *)Body.data()             ;
        double   v = name.toDouble(&okay)              ;
        if (okay)                                      {
          d[i] = v                                     ;
          name = QString::number(v,'f',Precision)      ;
          ItemEditing->setText(1,name)                 ;
        }                                              ;
      }                                                ;
    break                                              ;
    case N::Cpp::LDouble                               :
      if (name.length()>0)                             {
        bool    okay = true                            ;
        long double * d = (long double *)Body.data()   ;
        double  v = name.toDouble(&okay)               ;
        if (okay)                                      {
          d[i] = (long double)v                        ;
          name = QString::number(v,'f',Precision)      ;
          ItemEditing->setText(1,name)                 ;
        }                                              ;
      }                                                ;
    break                                              ;
  }                                                    ;
  removeOldItem()                                      ;
  Alert ( Click )                                      ;
}
