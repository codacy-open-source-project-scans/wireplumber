.. _si_interfaces_api:

Session Items Interfaces
========================
.. graphviz::
  :align: center

   digraph inheritance {
      rankdir=LR;
      GInterface -> WpSiAdapter;
      GInterface -> WpSiLinkable;
      GInterface -> WpSiLink;
      GInterface -> WpSiAcquisition;
   }

.. doxygenstruct:: WpSiAdapter

.. doxygenstruct:: _WpSiAdapterInterface

.. doxygenstruct:: WpSiLinkable

.. doxygenstruct:: _WpSiLinkableInterface

.. doxygenstruct:: WpSiLink

.. doxygenstruct:: _WpSiLinkInterface

.. doxygenstruct:: WpSiAcquisition

.. doxygenstruct:: _WpSiAcquisitionInterface

.. doxygengroup:: wpsiinterfaces
   :content-only:
